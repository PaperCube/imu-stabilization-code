#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// Reference: https://github.com/PengKun-PK/SerialPortCommunication

std::string as_hex(unsigned char val) {
    static char buf[3];
    sprintf(buf, "%02X", val);
    return std::string(buf);
}

std::vector<std::string> query_com_ports() {
    using namespace std;

    vector<string> ports;
    for (int i = 0; i < 255; i++) {
        char port_name[32];
        char target_path[4096];
        sprintf(port_name, "COM%d", i);
        if (!QueryDosDeviceA(port_name, target_path, sizeof(target_path))) {
            // printf("QueryDosDeviceA failed with error %d.\n",
            //        (int)GetLastError());
            continue;
        }
        ports.push_back(port_name);
    }
    return ports;
}

void print_ports() {
    printf("Available COM ports:\n");
    for(auto& port : query_com_ports()) {
        printf("%s\n", port.c_str());
    }
    printf("\n\n");
}

int main() {
    print_ports();

    HANDLE hCom = CreateFileA(R"(\\.\COM6)", GENERIC_READ | GENERIC_WRITE, 0,
                              NULL, OPEN_EXISTING, 0, NULL);

    // SetupComm(hCom, 1024, 1024);

    DCB dcb;
    GetCommState(hCom, &dcb);
    dcb.BaudRate = 19200;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    SetCommState(hCom, &dcb);

    PurgeComm(hCom,
              PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    DWORD dwError;
    if (!ClearCommError(hCom, &dwError, NULL)) {
        printf("ClearCommError failed with error %d.\n", (int)GetLastError());
    }

    std::ofstream file("output.bin", std::ios::binary);

    auto addr = 0LL;
    std::string discard;
    do {
        const int LINE_WIDTH = 11;
        const int LINES = 100;
        char readBuf[LINE_WIDTH * LINES];

        if (!ReadFile(hCom, readBuf, sizeof(readBuf), NULL, NULL)) {
            printf("ReadFile failed with error %d.\n", (int)GetLastError());
            return 1;
        }

        file.write(readBuf, sizeof(readBuf));

        for (unsigned i = 0; i < std::size(readBuf); i++) {
            if (i % LINE_WIDTH == 0) {
                printf("%08X: ", (int)addr);
                addr += LINE_WIDTH;
            }
            std::cout << (readBuf[i] == 0x55 ? "**" : as_hex(readBuf[i]))
                      << " ";
            if (i % LINE_WIDTH == LINE_WIDTH - 1) {
                std::cout << std::endl;
            }
        }
        printf("Press enter to continue...\n");
    } while (std::getline(std::cin, discard));
}