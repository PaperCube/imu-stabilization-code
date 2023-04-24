
#ifndef STABILIZER_PROTOTYPE_HOMOGRAPHY_H
#define STABILIZER_PROTOTYPE_HOMOGRAPHY_H

#include <opencv2/opencv.hpp>

namespace stabilizer {
    void normalize_l2(cv::InputOutputArray);
    cv::Mat get_cross_product_matrix(cv::InputArray);
    cv::Mat get_rodrigue_rotation_matrix(cv::InputArray, double);
    cv::Mat rodrigue_rotate(cv::InputArray, cv::InputArray, double);
}

#endif //STABILIZER_PROTOTYPE_HOMOGRAPHY_H
