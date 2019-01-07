//
// Created by Wavy on 27.11.2018.
//

#include "ImageProcessor.h"
#include <iostream>
#include <iomanip>
#include <shared_mutex>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "Util.h"

using namespace cv;
using namespace std;

// Declare the output variables
cv::Mat dst, cdst, cdstP, src, src_gray, firstFrame;

const int cameraNo = 1;
const int sensitivity_max = 255;
const int hexagon_area_slider_max_ = 100000;

int hexagon_max_box_area = 74000;
int hexagon_min_box_area = 4110;
int movement_sensitivity = 0;
int movement_threshold = 0;

int box_sensitivity_threshold = 56;
int box_sensitivity_threshold_max = 120;
int min_line_length = 20;
int saturation_cast = 2;
int color_selection_border = 10;

int max_line_gap = 15;
int line_sensitivity_threshold = 10;
int unti_length = 80;
int hexagon_length = 80;

bool isMoving = false;

std::vector<Hexagon> hexagons1;

const char *const contour_window = "Contours";
const char *const source_window = "Source";
const char *const parameter_window = "Parameters";
const char *const lines_window = "Detected Lines (in red) - Probabilistic Line Transform";
const char *const background_subtract_window = "Background Subtracted";
vector<RotatedRect> minRect;

int delay = 10;

void backgroundSubtraction_callback(int, void *);

void boundingBoxes_callback(int, void *);

int ImageProcessor::run() {
    VideoCapture cap(cameraNo); // open the default camera
    if (!cap.isOpened()) {
        return -1;
    }
    ImageProcessor::setupGUI();

    for (;;) {
        cap >> src;

        // Show results
        namedWindow(source_window, WINDOW_AUTOSIZE);
        imshow(source_window, src);

        if (ImageProcessor::detectEdges()) {
            ImageProcessor::analyzeHexagons();
            backgroundSubtraction_callback(movement_sensitivity, 0);
            boundingBoxes_callback(box_sensitivity_threshold, 0);
        }

        // Wait and Exit
        writeHexagonFile(hexagons1);
        waitKey(delay);
    }
}

void ImageProcessor::setupGUI() {
    namedWindow(parameter_window, WINDOW_FREERATIO);
    namedWindow(contour_window, WINDOW_AUTOSIZE);
    namedWindow(lines_window, WINDOW_AUTOSIZE);
    namedWindow(background_subtract_window, WINDOW_AUTOSIZE);
    createTrackbar("Line Sensitivity Threshold", parameter_window, &line_sensitivity_threshold, sensitivity_max,
                   nullptr);
    createTrackbar("Max Line Gap", parameter_window, &max_line_gap, sensitivity_max, nullptr);
    createTrackbar("Min Line Length", parameter_window, &min_line_length, sensitivity_max, nullptr);
    createTrackbar("Box Sensitivity:", parameter_window, &box_sensitivity_threshold, box_sensitivity_threshold_max,
                   boundingBoxes_callback);
    createTrackbar("Threshold subtraction:", parameter_window, &movement_sensitivity, box_sensitivity_threshold_max,
                   backgroundSubtraction_callback);
    createTrackbar("Area Max", parameter_window, &hexagon_max_box_area, hexagon_area_slider_max_, nullptr);
    createTrackbar("Area Min", parameter_window, &hexagon_min_box_area, hexagon_area_slider_max_, nullptr);
    createTrackbar("Unit length", parameter_window, &unti_length, sensitivity_max, nullptr);
    createTrackbar("Hexagon length", parameter_window, &hexagon_length, sensitivity_max, nullptr);
    createTrackbar("Movement Threshold", parameter_window, &movement_threshold, 40, nullptr);
    createTrackbar("Saturation Cast", parameter_window, &saturation_cast, sensitivity_max, nullptr);
    createTrackbar("Color Border", parameter_window, &color_selection_border, 40, nullptr);
}


void ImageProcessor::writeHexagonFile(std::vector<Hexagon> data) {
    nlohmann::json hexagons = nlohmann::json::array();
    for (int i = 0; i < data.size(); i++) {
        hexagons.push_back(data.at(i).toJSON());
    }
    nlohmann::json json = nlohmann::json();
    json["Hexagons"] = hexagons;
    std::string print = "";
    print.append("./info");
    print.append(".json");
    std::ofstream o(print);

    o << json << std::endl;
    o.close();
}


bool ImageProcessor::detectEdges() {
    // Edge detection, not relevant for
    Canny(src, dst, 50, 200, 3);

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    blur(src_gray, src_gray, Size(3, 3));

    // Copy edges to the images that will display the results in BGR
    // Probabilistic Line Transform
    cvtColor(dst, cdst, COLOR_GRAY2BGR);
    return true;
}

void ImageProcessor::analyzeHexagons() {
    std::vector<Hexagon> hexagons = std::vector<Hexagon>();
    cdstP = cdst.clone();

    Scalar color;
    int red = 0;
    int blue = 0;
    int green = 0;
    int sum = 0;

    // Draw the lines
    for (size_t j = 0; j < minRect.size(); j++) {

        Point2f rect_points[4];
        minRect[j].points(rect_points);

        float edge1 = Util::euclideanDist(rect_points[0], rect_points[1]);
        float edge2 = Util::euclideanDist(rect_points[1], rect_points[2]);
        float boxArea = edge1 * edge2;

        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            if (!isMoving) {
                Hexagon hexagon;
                calcHexagonColor(red, blue, green, sum, edge1, edge2, &minRect[j], &hexagon);
                calcHexagonPosition(&minRect[j], &hexagon);
                hexagons.push_back(hexagon);
            }
        }
    }
    hexagons1 = hexagons;
    cv::imshow(lines_window, cdstP);
}

void
ImageProcessor::calcHexagonColor(int red, int blue, int green, int sum, float edge1, float edge2, cv::RotatedRect *rect,
                                 Hexagon *hexagon) {
    red = 0;
    blue = 0;
    green = 0;
    sum = 0;
    for (int y = color_selection_border; y < edge1 - color_selection_border; y++) {
        for (int x = color_selection_border; x < edge2 - color_selection_border; x++) {
            int posX = static_cast<int>(y + rect->center.y - edge1 / 2);
            int posY = static_cast<int>(x + rect->center.x - edge2 / 2);
            Vec3b pixel = src.at<Vec3b>(posX, posY);
            red += pixel[2];
            green += pixel[1];
            blue += pixel[0];
            sum++;
            cdstP.at<Vec3b>(posX, posY) = pixel;
        }
    }
    red = red / sum;
    blue = blue / sum;
    green = green / sum;
    std::stringstream stream;
    stream << setfill('0') << setw(2) << std::hex << red;
    stream << setfill('0') << setw(2) << std::hex << green;
    stream << setfill('0') << setw(2) << std::hex << blue;
    hexagon->setColor(stream.str());
}

// Calculates the Position of
void ImageProcessor::calcHexagonPosition(cv::RotatedRect *rect, Hexagon *hexagon) {
    cv::Point2i gridPosition;
    gridPosition = hexagon->mapScreenToGridPosition(rect->center, gridPosition, hexagon_length);
    hexagon->setX(gridPosition.x);
    hexagon->setY(gridPosition.y);
}

void backgroundSubtraction_callback(int, void *) {
    Mat *input = &src;
    Mat mask(input->rows, input->cols, CV_8UC1);
    cvtColor(*input, mask, CV_RGB2GRAY);

    if (firstFrame.cols == 0) {
        mask.copyTo(firstFrame);
    }
    absdiff(mask, firstFrame, mask);
    cv::threshold(mask, mask, movement_sensitivity, 255, CV_THRESH_BINARY);
    Moments m = moments(mask, true);
    Point centerOfMass(static_cast<int>(m.m10 / m.m00), static_cast<int>(m.m01 / m.m00));
    Mat output(input->rows, input->cols, CV_8UC3, Scalar(255, 255, 0));
    input->copyTo(output, mask);
    cv::Rect rect = cv::Rect(mask.cols / 2 - movement_threshold / 2, mask.rows / 2 - movement_threshold / 2,
                             movement_threshold, movement_threshold);
    circle(mask, centerOfMass, 5, Scalar(0, 0, 255), CV_FILLED);
    isMoving = !centerOfMass.inside(rect);
    imshow("Background Subtracted", mask);
}


void boundingBoxes_callback(int, void *) {
    RNG rng(12345);
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    threshold(src_gray, threshold_output, box_sensitivity_threshold, 255, THRESH_BINARY);
    findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    minRect = vector<RotatedRect>(contours.size());
    vector<RotatedRect> hexagonsWithRightSize;
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

        float boxArea = (Util::euclideanDist(rect_points[0], rect_points[1]) *
                         Util::euclideanDist(rect_points[2], rect_points[3]));

        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            hexagonsWithRightSize.push_back(minRect[i]);
            for (int j = 0; j < 4; j++) {
                line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
            }
        }
    }
    if (!isMoving) {
        minRect = hexagonsWithRightSize;
    }
    imshow(contour_window, drawing);
}