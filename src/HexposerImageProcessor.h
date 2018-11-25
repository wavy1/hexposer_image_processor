//
// Created by Wavy on 15.11.2018.
//

#ifndef HEXAPOSER_HEXPOSERIMAGEPROCESSOR_H
#define HEXAPOSER_HEXPOSERIMAGEPROCESSOR_H

#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <c++/iostream>
#include <opencv2/videoio.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

class HexposerImageProcessor {
public:
    HexposerImageProcessor();
    virtual ~HexposerImageProcessor();
    bool setupGUI();
    void detectLines();
    bool detectEdges();
    float euclideanDist(cv::Point2f &p, cv::Point2f &q);
    void boundingBoxes_callback(int, void *);
    void backgroundSubtraction_callback(int, void *);

    int run();

private:
    cv::Mat dst, cdst, cdstP, src, src_gray, firstFrame;
    const int cameraNo = 1;
    int box_sensitivity_threshold;
    int box_sensitivity_threshold_max;
    int movement_threshold;
    int min_line_length;
    int max_line_gap;
    int line_sensitivity_threshold;
    int hexagon_max_box_area;
    int hexagon_min_box_area;
    int unti_length;
    int const sensitivity_max = 255;
    int const hexagon_area_slider_max_ = 100000;
    cv::String const srcWindowName = "S";
    std::vector<cv::RotatedRect> minRect;
    int frameRate;
    void setVars();
};

#endif //HEXAPOSER_HEXPOSERIMAGEPROCESSOR_H
