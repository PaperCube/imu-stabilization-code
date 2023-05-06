#include "com_device.h"
#include "win32utils.h"
#include <windows.h>
#include <thread>
#include <cstdio>
#include <functional>
#include <cassert>

ComDevice::ComDevice(const int aComPort, const int aBaudRate) : ComDevice() {
    open(aComPort, aBaudRate);
}

ComDevice::ComDevice() {

}

ComDevice::ComDevice(ComDevice &&other) noexcept {
    swap(other);
}

ComDevice &ComDevice::operator=(ComDevice &&other) noexcept {
    swap(other);
    return *this;
}

void ComDevice::swap(ComDevice &other) {
    // todo implement this method
}

static bool alterCommState(HANDLE handle, const std::function<void(DCB &)> &alter) {
    DCB dcb;
    int result = GetCommState(handle, &dcb);
    if (!result) {
        return false;
    }
    alter(dcb);
    return SetCommState(handle, &dcb) != 0;
}

bool ComDevice::open(const int aComPort, const int aBaudRate) {
    bool ok = false;
    mComPort = aComPort;
    mBaudRate = aBaudRate;
    wchar_t port_name[32] = L"\\\\.\\COM";
    swprintf(port_name + 7, L"%d", mComPort);

    DWORD dwResult;
    DCB dcb;

    mHandleComDevice = CreateFileW(
            port_name,
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            0,
//            FILE_FLAG_OVERLAPPED, // if running in synchronous mode, this should not be marked overlapped
            nullptr);
    if (mHandleComDevice == INVALID_HANDLE_VALUE) {
        dwResult = GetLastError();
        return false;
    }

    ok = SetupComm(mHandleComDevice, kSystemBufferSize, kSystemBufferSize);
    if (!ok) return false;

    ok = alterCommState(mHandleComDevice, [this](DCB &dcb) {
        dcb.BaudRate = this->mBaudRate;
        dcb.fParity = NOPARITY;
        dcb.ByteSize = 8;
        dcb.fDtrControl = DTR_CONTROL_ENABLE; // DTR = 0;接收
        dcb.fRtsControl = RTS_CONTROL_ENABLE; // RTS = 0;接收
        dcb.StopBits = ONESTOPBIT;
    });
    assert(ok);

    COMSTAT stat;
    DWORD dwErrorFlags;
    ClearCommError(mHandleComDevice, &dwErrorFlags, &stat);

    dwResult = GetLastError();
    mFlagOpened = true;
    return true;
}

ComDevice::~ComDevice() {
    close();
}

void ComDevice::close() {
    PurgeComm(mHandleComDevice,
              PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    CloseHandle(mHandleComDevice);
    mFlagOpened = false;
}

bool ComDevice::flush() {
    return FlushFileBuffers(mHandleComDevice);
}

void ComDevice::setBaudRate(int aBaudRate) {
    mBaudRate = aBaudRate;
    alterCommState(mHandleComDevice, [&](DCB &dcb) {
        dcb.BaudRate = mBaudRate;
    });
}

long ComDevice::read(unsigned char *aBuffer, size_t aSize, DWORD *aOutErr) {
    DWORD dwBytesRead = 0;
    int ok = ReadFile(mHandleComDevice, aBuffer, aSize, &dwBytesRead, nullptr);
    DWORD err = GetLastError();
    if (aOutErr) *aOutErr = err;
    if (err) {
        reportWin32Error(err);
    }
    return ok ? dwBytesRead : -1;
}

long ComDevice::write(const unsigned char *aBuffer, size_t aSize, DWORD *aOutErr) {
    alterCommState(mHandleComDevice, [&](DCB &dcb) {
        dcb.fDtrControl = DTR_CONTROL_DISABLE; // Not sure why this is necessary
        // Maybe this is a feature of the chip
    });

    DWORD dwBytesWritten = 0;
    int ok = WriteFile(mHandleComDevice, aBuffer, aSize, &dwBytesWritten, nullptr);

    alterCommState(mHandleComDevice, [&](DCB &dcb) {
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
    });

    assert(aSize == dwBytesWritten);
    DWORD err = GetLastError();
    if (aOutErr) *aOutErr = err;
    if (err) {
        reportWin32Error(err);
    }
//    printf("written %d bytes\n", int(dwBytesWritten));
    return ok ? dwBytesWritten : -1;
}
