//
// Created by Wavy on 27.11.2018.
//

#ifndef HEXAPOSER_UTIL_H
#define HEXAPOSER_UTIL_H

#include <opencv2/core/types.hpp>

class Util {
public:
    static float euclideanDist(cv::Point2f &p, cv::Point2f &q);
};


#endif //HEXAPOSER_UTIL_H
