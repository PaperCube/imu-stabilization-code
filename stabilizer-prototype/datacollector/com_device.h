
#ifndef STABILIZER_PROTOTYPE_COM_DEVICE_H
#define STABILIZER_PROTOTYPE_COM_DEVICE_H

#include <windows.h>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

class ComDevice {
    friend class SensorDevice;

private:
    static const int kSystemBufferSize = 8;

    HANDLE mHandleComDevice = nullptr;
    volatile bool mFlagOpened = false;
    int mComPort, mBaudRate;

    std::mutex mCloseLock;

public:
    ComDevice(int aComPort, int aBaudRate);
    ComDevice();

    ComDevice(const ComDevice &) = delete;
    ComDevice(ComDevice &&) noexcept;
    ComDevice &operator=(const ComDevice &) = delete;
    ComDevice &operator=(ComDevice &&) noexcept;

    void swap(ComDevice &other);

    bool opened() const { return mFlagOpened; }

    explicit operator bool() const { return opened(); }

    bool open(int com_port, int baud_rate);
    void close();

    int baudRate() const {
        return mBaudRate;
    }

    void setBaudRate(int);
    bool flush();

    long read(unsigned char *buffer, size_t size, DWORD *aOutErr = nullptr);
    long write(const unsigned char *buffer, size_t size, DWORD *aOutErr = nullptr);

    ~ComDevice();
};

#endif // STABILIZER_PROTOTYPE_COM_DEVICE_H
