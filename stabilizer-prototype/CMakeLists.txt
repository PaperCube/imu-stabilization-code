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

        utils/homography.h
        utils/homography.cpp

        utils/quaternion.h
        utils/quaternion.cpp
        utils/quaternion_cv.h
)

target_link_libraries(stabilizer_prototype ${OpenCV_LIBS})

