#include <cstdio>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>

#include <opencv2/opencv.hpp>

#include "win32utils.h"
#include "sensor_device.h"
#include "data_collector.h"

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using cv::VideoCapture;
using cv::VideoWriter;

mutex gPerfLoggerMutex, gPerfLoggerTimerMutex;
fstream gPerformanceLogger("log-perf.txt", ios::out);
auto gLastTime = steady_clock::now();

void logTimeHeader() {
    lock_guard<mutex> g(gPerfLoggerTimerMutex);
    auto now = steady_clock::now();
    auto elapsed = duration_cast<nanoseconds>(now - gLastTime).count();
    gLastTime = now;

    gPerformanceLogger << "[+" << fixed << setw(12) << setprecision(9) << (long double) (elapsed) / 1e9 << "] ";
}

template<typename... Args>
void logEvent(const Args &...values) {
    lock_guard<mutex> g(gPerfLoggerMutex);
    logTimeHeader();
    (gPerformanceLogger << ... << values); // requires C++ 17
    gPerformanceLogger << endl;
}

namespace collector_impl {
    const string kVideoFileName = "video_sample.avi";
    const string kGyroFileName = "gyro.txt";
    const string kFrameStampFileName = "framestamps.txt";

    const auto kProgramStart = steady_clock::now();

    fstream gGyroOutput(kGyroFileName, ios::out);
    fstream gFrameStampOutput(kFrameStampFileName, ios::out);

    template<typename T, int S>
    ostream &writeArray(ostream &os, const T (&a)[S]) {
        for (int i = 0; i < S; i++) {
            os << a[i] << ",";
        }
        return os;
    }

    void afterMessageCallback(const SensorState &s) {
        auto now = steady_clock::now();
        auto elapsed = duration_cast<milliseconds>(now - kProgramStart).count();
        if (!gGyroOutput) return;
        gGyroOutput << (elapsed / 1000.0) << ',';
        writeArray(gGyroOutput, s.Acceleration); // 1-4
        writeArray(gGyroOutput, s.AngularVelocity); // 4-7
        writeArray(gGyroOutput, s.EulerAngle); // 7-10
        writeArray(gGyroOutput, s.MagneticField); // 10-13
        writeArray(gGyroOutput, s.Quaternion); // 13-17
        gGyroOutput << endl;
    }

    void runDataCollector() {
        SensorDevice sensor(9, 230400);

        printf("awaiting sensor response... \n");
        sensor.awaitDeviceFirstResponse(5);
        printf("received sensor response\n");

        sensor.performCalibration(SensorDevice::CalibrationMode::SetAngleReference);
        printf("set angle reference\n");

        sensor.setAfterMessageCallback(collector_impl::afterMessageCallback);

        cv::Mat mat;

        VideoCapture cap(0);

        if (!cap.isOpened()) {
            printf("failed to open camera\n");
            return;
        }
        cap >> mat;

        int vWidth = (int) cap.get(cv::CAP_PROP_FRAME_WIDTH);
        int vHeight = (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT);

        VideoWriter writer(
                collector_impl::kVideoFileName,
                VideoWriter::fourcc('M', 'J', 'P', 'G'),
                30,
                cv::Size(vWidth, vHeight)
        );

        bool started = false;

        cout << "Press 's' in the preview window to start recording" << endl;

        while (cap >> mat, cap.isOpened()) {
            logEvent("Received a frame");
            cv::imshow("Camera Preview", mat);
            logEvent("Displayed. ");

            if (started) {
                const auto elapsed = duration_cast<milliseconds>(
                        steady_clock::now() - collector_impl::kProgramStart).count(); // preferably moved before imshow?
                logEvent("Fetched and calculated timestamp");
                writer << mat;
                logEvent("Wrote the frame");
                collector_impl::gFrameStampOutput << (elapsed / 1000.0) << endl;
                logEvent("Wrote timestamp");
            }

            int key = cv::pollKey();

            if (!started && key == 's') {
                sensor.performCalibration(SensorDevice::CalibrationMode::SetAngleReference);
                printf("set angle reference\n");
                started = true;
            }

            if (key == 'q') {
                break;
            }
        }

        printf("Releasing...\n");
        cv::destroyAllWindows();
        writer.release();
        cap.release();
        gFrameStampOutput.close();
        sensor.close();
        printf("Exiting\n");

    }
}

