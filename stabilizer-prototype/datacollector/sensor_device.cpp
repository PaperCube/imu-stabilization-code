
#include "sensor_device.h"
#include "win32utils.h"
#include <cstring>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <algorithm>

using ubyte = unsigned char;

const unsigned char *SensorDevice::kUnlockMessage = (const unsigned char *) "\xFF\xAA\x69\x88\xB5";
const unsigned char *SensorDevice::kSaveMessage = (const unsigned char *) "\xFF\xAA\x00\x00\x00";


SensorDevice::SensorDevice() :
        mComDevice(),
        mProcessQueue(64) {
    init();
}

SensorDevice::SensorDevice(int aComPort, int aBaudRate) :
        mComDevice(aComPort, aBaudRate),
        mProcessQueue(64) {
    init();
    startMonitorThreadIfOk();
}

SensorDevice::SensorDevice(SensorDevice &&other) noexcept:
        mComDevice(std::move(other.mComDevice)),
        mProcessQueue(std::move(other.mProcessQueue)) {
    swap(other);
}

bool SensorDevice::open(int aComPort, int aBaudRate) {
    bool ok = mComDevice.open(aComPort, aBaudRate);
    if (!ok) return false;
    startMonitorThreadIfOk();
    return true;
}

void SensorDevice::close() {
    mComDevice.close();
    mMonitorThread->join();
    delete mMonitorThread;
    mMonitorThread = nullptr;
}

void SensorDevice::startMonitorThreadIfOk() {
    if (mMonitorThread) return;
    if (mComDevice.opened()) {
        mMonitorThread = new std::thread(&SensorDevice::monitorThreadWorker, this);
    }
}

void SensorDevice::init() {
    mBuffer = new unsigned char[kBlockSize];
}

void SensorDevice::monitorThreadWorker() {
    while (mComDevice.opened()) {
        DWORD readBytes = mComDevice.read(mBuffer, kBlockSize);
        if (readBytes > 0) {
            for (unsigned i = 0; i < readBytes; i++) {
                dataByteCallback(mBuffer[i]);
            }
        }
    }
}

static const unsigned char kRecognizedDataTypeBytes[] = {
        SensorMagics::DataTypeAcceleration,
        SensorMagics::DataTypeAngularVelocity,
        SensorMagics::DataTypeEulerAngle,
        SensorMagics::DataTypeMagneticField,
        SensorMagics::DataTypeQuaternion,
};

ubyte calculateCrc(ubyte *begin, ubyte *end) {
    ubyte crc = 0;
    for (auto it = begin; it != end; it++) {
        crc += *it;
    }
    return crc;
}

void SensorDevice::dataByteCallback(unsigned char aValue) {
    mProcessQueue.push_back(aValue);

    while (!mProcessQueue.empty() && mProcessQueue[0] != SensorMagics::ProtocolHeaderByte) {
        printf("Discarding malformed bytes %02X\n", mProcessQueue[0]);
        mProcessQueue.pop_front();
    }

    while (mProcessQueue.size() >= 11) {
        unsigned char bytes[11];
        mProcessQueue.copy_range(bytes, 11);
        if (bytes[10] != calculateCrc(bytes, bytes + 10)) {
            printf("invalid crc\n");
            while (!mProcessQueue.empty() && mProcessQueue[0] == SensorMagics::ProtocolHeaderByte) {
                mProcessQueue.pop_front();
            }
            continue;
        }
        mProcessQueue.shrink_left(11);
//        printf("Received and parsed correctly formed message\n");
        handleMessage(bytes);
    }
}

SensorDevice &SensorDevice::operator=(SensorDevice &&other) noexcept {
    swap(other);
    return *this;
}

template<typename OutputType>
void remapLittleEndianInt16ThenCopy(const ubyte *aBinarySource, OutputType *aOutput, int count, OutputType multiplier) {
    for (int i = 0; i < count; i++) {
        unsigned valueCompose = (unsigned(aBinarySource[2 * i + 1]) << 8) | aBinarySource[2 * i];
        auto value = (short) valueCompose;
        aOutput[i] = value * multiplier;
//        printf("%02x %02x (%7d) -> %f\n", aBinarySource[2 * i], aBinarySource[2 * i + 1], value, aOutput[i]);
    }
}

void SensorDevice::sParseMessageToSensorState(unsigned char *aMsg, SensorState &aSensorState) {
    const ubyte type = aMsg[1];
//    const ubyte al = aMsg[2], ah = aMsg[3];
//    const ubyte bl = aMsg[4], bh = aMsg[5];
//    const ubyte cl = aMsg[6], ch = aMsg[7];
//    const ubyte dl = aMsg[8], dh = aMsg[9];
    switch (type) {
        case SensorMagics::DataTypeAcceleration:
            remapLittleEndianInt16ThenCopy<double>(aMsg + 2, aSensorState.Acceleration, 3, 1.0 / 32768 * 16 * 9.8);
            break;
        case SensorMagics::DataTypeAngularVelocity:
            remapLittleEndianInt16ThenCopy<double>(aMsg + 2, aSensorState.AngularVelocity, 3, 1.0 / 32768 * 2000);
            break;
        case SensorMagics::DataTypeEulerAngle:
            remapLittleEndianInt16ThenCopy<double>(aMsg + 2, aSensorState.EulerAngle, 3, 1.0 / 32768 * 180);
            break;
        case SensorMagics::DataTypeMagneticField:
            remapLittleEndianInt16ThenCopy<double>(aMsg + 2, aSensorState.MagneticField, 3, 1.0);
            break;
        case SensorMagics::DataTypeQuaternion:
            remapLittleEndianInt16ThenCopy<double>(aMsg + 2, aSensorState.Quaternion, 4, 1.0 / 32768);
            break;
        case SensorMagics::DataTypeTime:
            std::copy(aMsg + 2, aMsg + 8, aSensorState.Time);
            aSensorState.Time[6] = *reinterpret_cast<short *>(aMsg + 8);
            break;
        default:
//            printf("unknown data type %x\n", type);
            break;
    }
}

void SensorDevice::parseMessage(unsigned char *aMsg) {
    // value readouts are delegated to other functions
    switch (aMsg[1]) {
        case SensorMagics::DataTypeRegister:
            printf("register value %02x %02x %02x %02x %02x %02x %02x %02x ...\n",
                   aMsg[2], aMsg[3], aMsg[4], aMsg[5],
                   aMsg[6], aMsg[7], aMsg[8], aMsg[9]);
            {
                std::unique_lock<std::mutex> g(mRegisterLock);
                mRegisterUpdated = true;
                memcpy(mRegister.Content, aMsg + 2, 8);
                mRegisterCondition.notify_all();
            }
            break;
        default:
            sParseMessageToSensorState(aMsg, mSensorState);
            break;
    }
}

void SensorDevice::setAfterMessageCallback(const MessageListenerCallback &callback) {
    std::lock_guard<std::mutex> l(mSensorStateLock);
    mAfterMessageCallback = callback;
}

bool SensorDevice::awaitDeviceFirstResponse(int aRetries) {
    using namespace std::chrono_literals;

    std::unique_lock<std::mutex> g(mRegisterLock);
    while (aRetries != 0) { // negative value = infinite
        mRegisterUpdated = false;
        requestRegisterValue(0x34);
        bool result = mRegisterCondition.wait_for(g, 100ms, [this] { return mRegisterUpdated; });
        if (!result) {
            --aRetries;
            continue;
        } else {
            return true;
        }
    }
    return false;
}


void SensorDevice::handleMessage(unsigned char *begin) {
    // todo thread safety
    assert((*begin == 0x55) && "Unexpected malformed message");
//    printf("msg type %x\n", begin[1]);
    {
        std::unique_lock<std::mutex> g(mSensorStateLock);
        const MessageListenerCallback afterMessageCallback = mAfterMessageCallback;
        parseMessage(begin);

        int currentMessageType = begin[1];
        // presuming that the sensor returns values in the order that type# increases

        if (currentMessageType < mLastMessageType && afterMessageCallback) {
            SensorState copyState = mSensorState;
            g.unlock();
            afterMessageCallback(copyState);
        } else {
            g.unlock();
        }

        mLastMessageType = currentMessageType; // not required to be thread safe
    }
}

void SensorDevice::swap(SensorDevice &other) {
    std::swap(mComDevice, other.mComDevice);
    std::swap(mMonitorThread, other.mMonitorThread);
    std::swap(mBuffer, other.mBuffer);
    mProcessQueue.swap(other.mProcessQueue);
}

SensorState SensorDevice::getCurrentSensorState() {
    MutexLockGuard g(mSensorStateLock);
    SensorState copy = mSensorState;
    return copy;
}

bool SensorDevice::sendUartMessage(const unsigned char *begin, size_t size) {
    DWORD error;
    long written = mComDevice.write(begin, size, &error);
    assert(mComDevice.flush());
    if (written != (long) size) {
        fprintf(stderr, "Failed to write %lld bytes, only %ld bytes written\n", size, written);
        return false;
    }
    if (error) {
        reportWin32Error(error, "Failed to write to COM port");
        return false;
    }
    return true;
}

bool SensorDevice::sendCommandUnlocked(const unsigned char *begin, size_t size) {
    return sendUartMessage(kUnlockMessage, 5) &&
           (std::this_thread::sleep_for(std::chrono::milliseconds(100)), true) &&
           sendUartMessage(begin, size) &&
           sendUartMessage(kSaveMessage, 5);
}

void SensorDevice::setBaudRate(int aBaudRate) {
    static const int kBaudRates[] = {
            -1,
            4800,
            9600,
            19200,
            38400,
            57600,
            115200,
            230400,
            460800,
            921600,
    };
    static const int kBaudRateCnt = sizeof(kBaudRates) / sizeof(int);

    int baudRateIdx = 0;
    for (; baudRateIdx < kBaudRateCnt; baudRateIdx++) {
        if (kBaudRates[baudRateIdx] == aBaudRate) break;
    }

    assert(baudRateIdx < kBaudRateCnt && "Invalid baud rate");

    ubyte command[5] = "\xFF\xAA\x04";
    reinterpret_cast<short *>(command + 3)[0] = (short) baudRateIdx;
    sendCommandUnlocked(command, 5);

    mComDevice.setBaudRate(aBaudRate);
}

/**
 * Report Types:
 *  <br> - 0001(0x01): 0.2Hz
 *  <br> - 0010(0x02): 0.5Hz
 *  <br> - 0011(0x03): 1Hz
 *  <br> - 0100(0x04): 2Hz
 *  <br> - 0101(0x05): 5Hz
 *  <br> - 0110(0x06): 10Hz
 *  <br> - 0111(0x07): 20Hz
 *  <br> - 1000(0x08): 50Hz
 *  <br> - 1001(0x09): 100Hz
 *  <br> - 1011(0x0B): 200Hz
 *  <br> - 1100(0x0C): Single Report
 *  <br> - 1101(0x0D): No Report
 * @param aReportRateType refer to report types
 */
void SensorDevice::setReportRate(int aReportRateType) {
    ubyte command[5] = "\xFF\xAA\x03";
    reinterpret_cast<short *>(command + 3)[0] = (short) aReportRateType;
    assert(sendCommandUnlocked(command, 5));
}

void SensorDevice::requestRegisterValue(short aDataType) {
    ubyte command[5] = "\xFF\xAA\x27";
    reinterpret_cast<short *>(command + 3)[0] = aDataType;
    assert(sendCommandUnlocked(command, 5));
}

void SensorDevice::performCalibration(CalibrationMode aCalibrationMode) {
    ubyte command[5] = "\xFF\xAA\x01";
    reinterpret_cast<short *>(command + 3)[0] = (short) aCalibrationMode;
    assert(sendCommandUnlocked(command, 5));
}

SensorDevice::~SensorDevice() {
    delete[] mBuffer;
    if (mMonitorThread) {
        mMonitorThread->join();
        delete mMonitorThread;
    }
}