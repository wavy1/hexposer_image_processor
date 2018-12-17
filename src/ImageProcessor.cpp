//
// Created by Wavy on 27.11.2018.
//

#include "ImageProcessor.h"
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "Util.h"

using namespace cv;
using namespace std;

// Declare the output variables
cv::Mat dst, cdst, cdstP, src, src_gray;

const int cameraNo = 0;
const int sensitivity_max = 255;
const int hexagon_area_slider_max_ = 100000;

int hexagon_max_box_area = 10000;
int hexagon_min_box_area = 100;

int box_sensitivity_threshold = 20;
int box_sensitivity_threshold_max = 255;

int min_line_length = 20;
int max_line_gap = 15;
int line_sensitivity_threshold = 10;
int unti_length = 80;

const char *const contour_window = "Contours";
const char *const source_window = "Source";
const char *const parameter_window = "Parameters";
const char *const lines_window = "Detected Lines (in red) - Probabilistic Line Transform";


vector<RotatedRect> minRect;
int frameRate = 10;

void writeHexagonFile(std::vector<Hexagon> data);
void boundingBoxes_callback(int, void *);

void ImageProcessor::setupGUI() {
    namedWindow(parameter_window, WINDOW_FREERATIO);
    namedWindow(contour_window, WINDOW_AUTOSIZE);
    namedWindow(lines_window, WINDOW_AUTOSIZE);
    createTrackbar("Line Sensitivity Threshold", parameter_window, &line_sensitivity_threshold, sensitivity_max, nullptr);
    createTrackbar("Max Line Gap", parameter_window, &max_line_gap, sensitivity_max, nullptr);
    createTrackbar("Min Line Length", parameter_window, &min_line_length, sensitivity_max, nullptr);
    createTrackbar("Box Sensitivity:", parameter_window, &box_sensitivity_threshold, box_sensitivity_threshold_max,
                   boundingBoxes_callback);
    createTrackbar("Area Max", parameter_window, &hexagon_max_box_area, hexagon_area_slider_max_, boundingBoxes_callback);
    createTrackbar("Area Min", parameter_window, &hexagon_min_box_area, hexagon_area_slider_max_, boundingBoxes_callback);
    createTrackbar("Unit length", parameter_window, &unti_length, sensitivity_max, nullptr);
}


void writeHexagonFile(std::vector<Hexagon> data){
    nlohmann::json hexagons = nlohmann::json();
    for(int i = 0; i < data.size(); i++) {
        hexagons.push_back(data.at(i).toJSON());
    }
    nlohmann::json json = nlohmann::json();
    json["hexagons"] = hexagons;
    std::string print = "";
    print.append("./pretty");
    print.append(std::to_string(0));
    print.append(".json");
    std::ofstream o(print);
    o << json << std::endl;
    o.close();
}


bool ImageProcessor::detectEdges() {
    // Edge detection
    Canny(src, dst, 50, 200, 3);

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    blur(src_gray, src_gray, Size(3, 3));

    // Copy edges to the images that will display the results in BGR
    // Probabilistic Line Transform
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    return true;
}

void ImageProcessor::detectLines() {
    std::vector<Hexagon> hexagons = std::vector<Hexagon>();
    bool isHexagonCounted = false;
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 30, CV_PI / 180, line_sensitivity_threshold, min_line_length,
                max_line_gap); // runs the actual detection
    cdstP = cdst.clone();

    string print = "";
    Scalar color;
    int red;
    int blue;
    int green;
    int sum;

    // Draw the lines
    for (size_t j = 0; j < minRect.size(); j++) {

        Point2f rect_points[4];
        minRect[j].points(rect_points);

        float edge1 = Util::euclideanDist(rect_points[0], rect_points[1]);
        float edge2 = Util::euclideanDist(rect_points[1], rect_points[2]);
        float boxArea = edge1 * edge2;

        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            for (size_t i = 0; i < linesP.size(); i++) {
                Vec4i l = linesP[i];
                if (minRect[j].boundingRect().contains(Point(l[0], l[1]))) {
                    line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
                    if(!isHexagonCounted){
                        red = 0;
                        blue = 0;
                        green = 0;
                        /*
                         sum = 0;
                        for(int y = 0; y < edge1; y++){
                            for(int x = 0; x < edge2; x++){
                                Vec3b pixel = src.at<Vec3b>(static_cast<int>(y + minRect[j].center.y - edge1 / 2),
                                                            static_cast<int>(x + minRect[j].center.x - edge2 / 2));
                                red += pixel[2];
                                green += pixel[1];
                                blue += pixel[0];
                                sum++;
                            }
                        }

                        red = red / sum;
                        blue = blue / sum;
                        green = green / sum;
                         */
                        std::stringstream stream;
                        stream << std::hex << red << std::hex << blue << std::hex << green;
                        Hexagon hexagon(static_cast<int>(minRect[j].center.x), static_cast<int>(minRect[j].center.y), stream.str());
                        hexagons.push_back(hexagon);
                        isHexagonCounted = true;
                    }
                }
            }
        }
        isHexagonCounted = false;
    }
    writeHexagonFile(hexagons);
    cv::imshow(lines_window, cdstP);
}

void boundingBoxes_callback(int, void *) {
    RNG rng(12345);
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    threshold(src_gray, threshold_output, box_sensitivity_threshold, 255, THRESH_BINARY);
    findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    minRect = vector<RotatedRect>(contours.size());
    vector<RotatedRect> minEllipse(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        minRect[i] = minAreaRect(contours[i]);
        if (contours[i].size() > 5) { minEllipse[i] = fitEllipse(contours[i]); }
    }
    Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        // contour
        drawContours(drawing, contours, (int) i, color, 1, 8, vector<Vec4i>(), 0, Point());
        // ellipse
        ellipse(drawing, minEllipse[i], color, 2, 8);
        // rotated rectangle
        Point2f rect_points[4];
        minRect[i].points(rect_points);

        float boxArea = (Util::euclideanDist(rect_points[0], rect_points[1]) * Util::euclideanDist(rect_points[2], rect_points[3]));

        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            for (int j = 0; j < 4; j++) {
                line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
            }
        }
    }
    imshow(contour_window, drawing);
}

int ImageProcessor::run(){
    VideoCapture cap(cameraNo); // open the default camera
    if (!cap.isOpened()){
        return -1;
    }  // check if we succeeded
    ImageProcessor::setupGUI();

    for (;;) {
        cap >> src;
        // Show results
        namedWindow(source_window, WINDOW_AUTOSIZE);
        imshow(source_window, src);

        if(ImageProcessor::detectEdges()){
            ImageProcessor::detectLines();
        }
        boundingBoxes_callback(box_sensitivity_threshold, 0);

        // Wait and Exit
        waitKey(frameRate);
    }
}