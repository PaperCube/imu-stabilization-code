#include <cstdio>
#include <cassert>
#include <chrono>
#include <fstream>

#include <opencv2/opencv.hpp>

#include "win32utils.h"
#include "sensor_device.h"
#include "data_collector.h"

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using cv::VideoCapture;
using cv::VideoWriter;

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
        gGyroOutput << (elapsed / 1000.0) << ',';
        writeArray(gGyroOutput, s.Acceleration);
        writeArray(gGyroOutput, s.AngularVelocity);
        writeArray(gGyroOutput, s.EulerAngle);
        writeArray(gGyroOutput, s.MagneticField);
        writeArray(gGyroOutput, s.Quaternion);
        gGyroOutput << endl;
    }

    void runDataCollector() {
        SensorDevice sensor(6, 230400);
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

        while (cap >> mat, cap.isOpened()) {
            const auto elapsed = duration_cast<milliseconds>(
                    steady_clock::now() - collector_impl::kProgramStart).count();
            writer << mat;
            collector_impl::gFrameStampOutput << (elapsed / 1000.0) << endl;
        }

    }
}

