#include <cstdio>
#include <cassert>
#include <chrono>
#include <fstream>

#include "datacollector/win32utils.h"
#include "datacollector/sensor_device.h"
#include "datacollector/data_collector.h"

using namespace std::chrono_literals;

void testComDevice() {
    SensorDevice sensor(6, 230400);
//    sensor.setBaudRate(230400);


//    auto &comDevice = sensor.comDevice();
//    ComDevice comDevice(6, 19200);
//    assert(comDevice.opened());
//    while (true) {
//        unsigned char c[64];
//        DWORD errCode;
//        long long read = comDevice.read(c, sizeof(c), &errCode);
//        if (read > 0) {
//            for (int i = 0; i < read; i++) {
//                printf("%02x ", c[i]);
//            }
//            printf("\n");
//        } else {
//            printf("read = %lld, err = %s\n", read, getErrorCodeString(errCode).c_str());
//            break;
//        }
//    }
//    printf("starting thread\n");
//    new std::thread([&](){
//        std::this_thread::sleep_for(3s);
//        sensor.requestRegisterValue(0x03);
//        sensor.setReportRate(0x09);
//        printf("written value\n");
//    });
//    printf("started thread\n");


    while (true) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);

        SensorState state = sensor.getCurrentSensorState();
        auto &acc = state.Acceleration;
        printf("%f %f %f\n", acc[0], acc[1], acc[2]);
//        fflush(stdout);
    }

    sensor.mMonitorThread->join();
}

using namespace std::chrono_literals;

void drawGraph(const double *values,
               int len,
               double aRatioFull = 20,
               int aColWidth = 21) {
    assert((aColWidth & 1) && "aColWidth must be odd");
    for (int i = 0; i < len; i++) {
        printf("%s", &"|"[int(i == 0)]);
        char buffer[128] = {};
        memset(buffer, ' ', aColWidth);
        const int halfWidth = aColWidth >> 1;
        buffer[halfWidth] = '|';
        int charsToFill = (int) std::trunc(values[i] / aRatioFull * halfWidth);
        for (int offset = 1; offset <= std::min(std::abs(charsToFill), halfWidth); offset++) {
            buffer[halfWidth + offset * (charsToFill < 0 ? -1 : 1)] = charsToFill < 0 ? '<' : '>';
        }
        printf("%s", buffer);
    }
//    printf("\n");
}

void testAngle() {
    SensorDevice sensor(6, 230400);
    assert(sensor.comDevice().opened());
    assert(sensor.awaitDeviceFirstResponse(5));
    while (true) {
        std::this_thread::sleep_for(50ms);

        SensorState state = sensor.getCurrentSensorState();
        auto &acc = state.Acceleration;
        drawGraph(acc, 3);
        printf(" | %f %f %f\n", acc[0], acc[1], acc[2]);
    }
}

namespace calibrator {
    int main();
}

int main() {
//    testComDevice();
//    collector_impl::runDataCollector();
    testAngle();
//    calibrator::main();
    return 0;
}
