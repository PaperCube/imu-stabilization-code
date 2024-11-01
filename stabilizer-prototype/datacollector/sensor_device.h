
#ifndef STABILIZER_PROTOTYPE_SENSOR_DEVICE_H
#define STABILIZER_PROTOTYPE_SENSOR_DEVICE_H

#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include "com_device.h"
#include <vector>
#include <condition_variable>
#include <initializer_list>
#include "fixed_size_buffer.h"

struct SensorState {
    short Time[7];
    double Acceleration[3];
    double AngularVelocity[3];
    double EulerAngle[3];
    double MagneticField[3];
    double Quaternion[4];
};

struct SensorRegister {
    unsigned char Content[8];
};

struct SensorMagics {
    enum : unsigned char {
        ProtocolHeaderByte = 0x55,

        DataTypeTime = 0x50,
        DataTypeAcceleration = 0x51,
        DataTypeAngularVelocity = 0x52,
        DataTypeEulerAngle = 0x53,
        DataTypeMagneticField = 0x54,
        DataTypeQuaternion = 0x59,
        DataTypeRegister = 0x5F,
    };
};

class SensorDevice {
public:
    enum class CalibrationMode : short {
        Normal = 0,
        AutoAccelerometer = 1,
        ClearAltitude = 3,
        ClearYaw = 4,
        AutoMagnetometerSphericalFit = 0x07,
        SetAngleReference = 0x08,
        AutoMagnetometerDualPlane = 0x09,
    };

private:
    ComDevice mComDevice;

public:
    static const unsigned char *kUnlockMessage;
    static const unsigned char *kSaveMessage;

    std::thread *mMonitorThread = nullptr;
    using MessageListenerCallback = std::function<void(const SensorState &)>;

    volatile bool mAllowCommandDelays = true;

private:
    unsigned char *mBuffer = nullptr;
    fixed_size_buffer<unsigned char> mProcessQueue;

    unsigned const kBlockSize = 8;

    SensorRegister mRegister{};
    std::mutex mRegisterLock;
    bool mRegisterUpdated = false;
    std::condition_variable mRegisterCondition;

    SensorState mSensorState = {};
    std::mutex mSensorStateLock;

    // locked on mSensorStateLock
    MessageListenerCallback mAfterMessageCallback;

    using MutexLockGuard = std::lock_guard<std::mutex>;

    int mLastMessageType = 0;

public:
    SensorDevice();
    SensorDevice(int aComPort, int aBaudRate);

    SensorDevice(const SensorDevice &) = delete;
    SensorDevice(SensorDevice &&) noexcept;
    SensorDevice &operator=(const SensorDevice &) = delete;
    SensorDevice &operator=(SensorDevice &&) noexcept;
    ~SensorDevice();
    void swap(SensorDevice &other);

    bool open(int aComPort, int aBaudRate);
    void close();

    ComDevice &comDevice() { return mComDevice; }

    void setAllowCommandDelays(bool aAllowCommandDelays) {
        mAllowCommandDelays = aAllowCommandDelays;
    }

    SensorState getCurrentSensorState();
//    SensorState awaitNextSensorState();

    void setAfterMessageCallback(const MessageListenerCallback &callback);

    bool awaitDeviceFirstResponse(int aRetries);

    bool sendUartMessage(const unsigned char *begin, size_t size);
    bool sendCommandUnlocked(const unsigned char *begin, size_t size);
    void setBaudRate(int aBaudRate);
    void setReportRate(int aReportRateType);
    void requestRegisterValue(short aDataType);
    void performCalibration(CalibrationMode aCalibrationMode);

private:
    void init();
    void startMonitorThreadIfOk();
    void monitorThreadWorker();
    void dataByteCallback(unsigned char);
    void handleMessage(unsigned char *begin);
    void parseMessage(unsigned char *aMsg);

    static void sParseMessageToSensorState(unsigned char *aMsg, SensorState &aOutState);
};

#endif //STABILIZER_PROTOTYPE_SENSOR_DEVICE_H
