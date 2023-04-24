#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

#include "utils/homography.h"
#include "utils/quaternion_cv.h"

int main() {
    using namespace std;
    using namespace cv;
    using namespace stabilizer;

//    vector<Point2f> arr{{0, 0}, {0, 1}, {1, 0}, {1, 1}};
//    Mat homography = getPerspectiveTransform(arr, arr);
//    cout << homography << endl;
    const double pi = acos(-1);

    Quaternion<double> q(0, 1, 0, 0);
    cout << q.rotate_point({0, 0, 0, 1}, pi / 2) << endl;

    return 0;
}
