
#ifndef STABILIZER_PROTOTYPE_COMDEVICE_H
#define STABILIZER_PROTOTYPE_COMDEVICE_H

#include <windows.h>
#include <functional>
#include <thread>
#include <atomic>

class ComDevice {
public:
    static const int buffer_size = 1000;

    using data_handler_t = std::function<void(unsigned char *, size_t)>;

private:
    HANDLE handle_com_device = nullptr;
    OVERLAPPED write_status, read_status;

    int com_port = -1, baud_rate = -1;
    unsigned char *receiver_buffer = nullptr;

    std::thread *thread;
    std::atomic_bool thread_running = false;

    bool flag_opened = false;

    void receiver_thread_runner();

public:
    data_handler_t receiver_handler;

    ComDevice(int com_port, int baud_rate);
    ComDevice();

    ComDevice(const ComDevice &) = delete;
    ComDevice(ComDevice &&) noexcept;
    ComDevice &operator=(const ComDevice &) = delete;
    ComDevice &operator=(ComDevice &&) noexcept;

    void swap(ComDevice &other);

    bool opened() const { return flag_opened; }
    explicit operator bool() const { return opened(); }

    bool open(int com_port, int baud_rate);

    bool send_uart_message(unsigned char *data, size_t length);
    void set_baud_rate(int baud_rate);

    ~ComDevice();
};

#endif // STABILIZER_PROTOTYPE_COMDEVICE_H
