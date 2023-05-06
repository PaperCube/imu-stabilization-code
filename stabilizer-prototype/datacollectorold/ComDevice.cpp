#include "ComDevice.h"
#include <windows.h>
#include <thread>
#include <cstdio>

ComDevice::ComDevice(const int com_port, const int baud_rate) : ComDevice() {
    open(com_port, baud_rate);
}

ComDevice::ComDevice() {
    receiver_buffer = new unsigned char[buffer_size];
}

ComDevice::ComDevice(ComDevice &&other) noexcept {
    swap(other);
}

ComDevice &ComDevice::operator=(ComDevice &&other) noexcept {
    swap(other);
    return *this;
}

void ComDevice::swap(ComDevice &other) {
    std::swap(handle_com_device, other.handle_com_device);
    std::swap(write_status, other.write_status);
    std::swap(read_status, other.read_status);
    std::swap(com_port, other.com_port);
    std::swap(baud_rate, other.baud_rate);
    std::swap(flag_opened, other.flag_opened);
    std::swap(thread, other.thread);
}

void ComDevice::receiver_thread_runner() {
    unsigned long uLen;
    DWORD dwRes;
    COMSTAT Comstat;
    DWORD dwErrorFlags;
    char chrBuffer[buffer_size] = {};

    while (thread_running) {
        if (!ReadFile(handle_com_device, chrBuffer, buffer_size - 1, &uLen,
                      &read_status)) {
            dwRes = GetLastError();
            if (dwRes != ERROR_IO_PENDING) {
                ClearCommError(handle_com_device, &dwErrorFlags, &Comstat);
                continue;
            }

            WaitForSingleObject(read_status.hEvent, INFINITE);
            if (!GetOverlappedResult(handle_com_device, &read_status, &uLen, false))
                continue;
            if (uLen <= 0)
                continue;

            if (receiver_handler) {
                receiver_handler(reinterpret_cast<unsigned char *>(chrBuffer), uLen);
            }
            continue;
        }

        if (uLen <= 0)
            continue;
    }
}

bool ComDevice::send_uart_message(unsigned char *data, size_t length) {
    DWORD bytes_transferred;
    DWORD dwRes;
    DCB dcb;

    GetCommState(handle_com_device, &dcb);
    dcb.fDtrControl = 0; // DTR = 1;发送
    SetCommState(handle_com_device, &dcb);

    if (WriteFile(handle_com_device, data, length, &bytes_transferred, &(write_status)) ||
        GetLastError() != ERROR_IO_PENDING)
        return false;
    dwRes = WaitForSingleObject(write_status.hEvent, 1000);
//    Sleep(10); // ?
    dcb.fDtrControl = 1; // DTR = 0;接收
    SetCommState(handle_com_device, &dcb);
//    Sleep(10);

    if (dwRes != WAIT_OBJECT_0 ||
        !GetOverlappedResult(handle_com_device, &write_status, &bytes_transferred, false))
        return false;
    return true;
}

void ComDevice::set_baud_rate(int _baud_rate) {
    this->baud_rate = _baud_rate;

    DCB dcb;
    GetCommState(handle_com_device, &dcb);
    dcb.BaudRate = this->baud_rate;
    SetCommState(handle_com_device, &dcb);
}

bool ComDevice::open(const int _com_port, const int _baud_rate) {
    com_port = _com_port;
    baud_rate = _baud_rate;
    wchar_t port_name[32] = L"\\\\.\\COM";
    swprintf(port_name + 7, L"%d", com_port);

    DWORD dwResult;
    DCB dcb;

    handle_com_device = CreateFileW(
            port_name,
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            nullptr);
    if (handle_com_device == INVALID_HANDLE_VALUE) {
        dwResult = GetLastError();
        return -1;
    }

    SetupComm(handle_com_device, buffer_size, buffer_size);

    GetCommState(handle_com_device, &dcb);
    dcb.BaudRate = this->baud_rate;
    dcb.fParity = NOPARITY;
    dcb.ByteSize = 8;
    dcb.fDtrControl = 0; // DTR = 0;接收
    dcb.fRtsControl = 0; // RTS = 0;接收
    dcb.StopBits = ONESTOPBIT;
    SetCommState(handle_com_device, &dcb);

    COMSTAT stat;
    DWORD dwErrorFlags;
    ClearCommError(handle_com_device, &dwErrorFlags, &stat);

    dwResult = GetLastError();

    // set timeout parameters
    COMMTIMEOUTS comTimeOut;
    comTimeOut.ReadIntervalTimeout = 5;
    comTimeOut.ReadTotalTimeoutMultiplier = 10;
    comTimeOut.ReadTotalTimeoutConstant = 100;
    comTimeOut.WriteTotalTimeoutMultiplier = 5;
    comTimeOut.WriteTotalTimeoutConstant = 5;
    SetCommTimeouts(handle_com_device, &comTimeOut);

    write_status.hEvent = CreateEvent(nullptr, true, false, nullptr);
    read_status.hEvent = CreateEvent(nullptr, true, false, nullptr);
    read_status.Internal = 0;
    read_status.InternalHigh = 0;
    read_status.Offset = 0;
    read_status.OffsetHigh = 0;


//    hCOMThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) ReceiveCOMData,
//                              nullptr, 0, NULL);
    thread_running = true;
    thread = new std::thread(&ComDevice::receiver_thread_runner, this);
//    Sleep(200); // ?

    flag_opened = true;
    return true;
}

ComDevice::~ComDevice() {
    thread_running = false;
    thread->join();
    thread->native_handle();
    delete thread;
    PurgeComm(handle_com_device,
              PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    CloseHandle(read_status.hEvent);
    CloseHandle(write_status.hEvent);
    CloseHandle(handle_com_device);
    delete[] receiver_buffer;
}

