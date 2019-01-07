//
// Created by Wavy on 27.11.2018.
//

#ifndef HEXAPOSER_IMAGEPROCESSOR_H
#define HEXAPOSER_IMAGEPROCESSOR_H

#include <opencv2/core/types.hpp>
#include <thread>
#include "Hexagon.h"

class ImageProcessor{
public:
    void setupGUI();
    void analyzeHexagons();
    bool detectEdges();
    int run();
    void writeHexagonFile(std::vector<Hexagon> data);
private:
    void calcHexagonColor(int red, int blue, int green, int sum, float edge1, float edge2, cv::RotatedRect *rect, Hexagon *hexagon);
    void calcHexagonPosition(cv::RotatedRect *rect, Hexagon *hexagon);
};


#endif //HEXAPOSER_IMAGEPROCESSOR_H
