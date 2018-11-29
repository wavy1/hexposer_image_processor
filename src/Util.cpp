//
// Created by Wavy on 27.11.2018.
//

#include "Util.h"

float Util::euclideanDist(cv::Point2f &p, cv::Point2f &q) {
    cv::Point diff = p - q;
    return (float) cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}