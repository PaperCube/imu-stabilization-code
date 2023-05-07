cmake_minimum_required(VERSION 3.25)
project(stabilizer_prototype)

set(CMAKE_CXX_STANDARD 17)

# Begin OpenCV import
set(OpenCV_BUILD "D:/Programs/_Libraries/opencv-msys2-qt-build-4.5.5/install")

add_library(opencv SHARED IMPORTED)
add_compile_options(-Wall -Wextra -g3)
list(APPEND CMAKE_PREFIX_PATH "${OpenCV_BUILD}")
find_package(OpenCV REQUIRED)
# End OpenCV import

add_executable(
        stabilizer_prototype
        main.cpp

        datacollector/data_collector.h

        utils/homography.h
        utils/homography.cpp

        utils/quaternion.h
        utils/quaternion.cpp
        utils/quaternion_cv.h
        datacollector/witsdk.h
        # end of executables
        datacollector/witsdk_constants.h datacollector/witsdk.cpp datacollector/com_device.cpp datacollector/com_device.h datacollector/sensor_device.cpp datacollector/sensor_device.h datacollector/fixed_size_buffer.h datacollector/win32utils.h datacollector/win32utils.cpp datacollector/data_collector.cpp)

target_link_libraries(stabilizer_prototype ${OpenCV_LIBS})

