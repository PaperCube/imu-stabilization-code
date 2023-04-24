#include "homography.h"

#include <opencv2/core/base.hpp>

#include <cmath>

namespace stabilizer {
    void normalize_l2(cv::InputOutputArray arr) {
        double norm = cv::norm(arr);
        arr.getMat() /= norm;
    }

    cv::Mat get_cross_product_matrix(
            cv::InputArray arr
    ) {
        cv::Mat_<double> _arr;
        arr.getMat().convertTo(_arr, CV_64F);

        // Get cross-product matrix
        double a1 = _arr[0][0], a2 = _arr[1][0], a3 = _arr[2][0];
        cv::Mat_<double> result({0, -a3, a2, a3, 0, -a1, -a2, a1, 0});

        cv::Mat ret;
        result.reshape(3, 3).convertTo(ret, arr.type());
        return ret;
    }

    cv::Mat get_rodrigue_rotation_matrix(
            cv::InputArray axis,
            double theta
    ) {
        cv::Mat normalized_axis;
        axis.copyTo(normalized_axis);
        normalize_l2(normalized_axis);

        auto k = get_cross_product_matrix(axis);
        auto i = cv::Mat::eye(3, 3, k.type());
        return i + std::sin(theta) * k + (1 - std::cos(theta)) * (k * k);
    }

    cv::Mat rodrigue_rotate(
            cv::InputArray v,
            cv::InputArray axis,
            double angle
    ) {
        return v.getMat() * get_rodrigue_rotation_matrix(axis, angle);
    }
}
