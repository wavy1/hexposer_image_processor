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
    void detectLines();
    bool detectEdges();
    int run();
};


#endif //HEXAPOSER_IMAGEPROCESSOR_H
