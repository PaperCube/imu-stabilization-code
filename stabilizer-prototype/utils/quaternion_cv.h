#pragma once

#include <opencv2/core/mat.hpp>
#include "quaternion.h"

template<typename T>
inline cv::Mat_<T> as_cv_mat(Quaternion<T> t) {
    cv::Mat_<T> result;
    result << t.a, t.b, t.c, t.d;
    return result;
}

template <typename T>
inline cv::Mat_<T> as_left_quaternion_matrix(Quaternion<T> q) {
    cv::Mat_<T> result;
    result << q.a, -q.b, -q.c, -q.d,
            q.b, q.a, -q.d, q.c,
            q.c, q.d, q.a, -q.b,
            q.d, -q.c, q.b, q.a;
    return result.reshape(4, 4);
}

template <typename T>
inline cv::Mat_<T> as_right_quaternion_matrix(Quaternion<T> q) {
    cv::Mat_<T> result;
    result << q.a, -q.b, -q.c, -q.d,
            q.b, q.a, q.d, -q.c,
            q.c, -q.d, q.a, q.b,
            q.d, q.c, -q.b, q.a;
    return result.reshape(4, 4);
}