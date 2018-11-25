//
// Created by Wavy on 15.11.2018.
//

#include "HexposerImageProcessor.h"

using namespace cv;
using namespace std;

int HexposerImageProcessor::run() {
    VideoCapture cap(cameraNo); // open the default camera
    if (!cap.isOpened()) {
        return -1;
    }  // check if we succeeded
    if (setupGUI()) {
        for (;;) {
            cap >> src;
            // Show results
            imshow("S", src);
            if (detectEdges()) {
                detectLines();
            }
            backgroundSubtraction_callback(movement_threshold, 0);
            boundingBoxes_callback(box_sensitivity_threshold, 0);

            // Wait and Exit
            waitKey(frameRate);
        }
    };
}

float HexposerImageProcessor::euclideanDist(cv::Point2f &p, cv::Point2f &q) {
    Point diff = p - q;
    return (float) cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}

bool HexposerImageProcessor::setupGUI() {
    cv::namedWindow(srcWindowName, cv::WINDOW_NORMAL);
    cv::createTrackbar("Line Sensitivity Threshold", srcWindowName, &line_sensitivity_threshold, sensitivity_max,
                       nullptr);
    cv::createTrackbar("Max Line Gap", srcWindowName, &max_line_gap, sensitivity_max, nullptr);
    cv::createTrackbar("Min Line Length", srcWindowName, &min_line_length, sensitivity_max, nullptr);
    cv::createTrackbar("Threshold subtraction:", srcWindowName, &movement_threshold, box_sensitivity_threshold_max,
                       nullptr);
    cv::createTrackbar("Box Sensitivity:", srcWindowName, &box_sensitivity_threshold, box_sensitivity_threshold_max,
                       nullptr);
    cv::createTrackbar("Area Max", srcWindowName, &hexagon_max_box_area, hexagon_area_slider_max_, nullptr);
    cv::createTrackbar("Area Min", srcWindowName, &hexagon_min_box_area, hexagon_area_slider_max_, nullptr);
    cv::createTrackbar("Unit length", srcWindowName, &unti_length, sensitivity_max, nullptr);
    return true;
}

void HexposerImageProcessor::backgroundSubtraction_callback(int, void *) {
    Mat *input = &src;
    Mat mask(input->rows, input->cols, CV_8UC1);
    cvtColor(*input, mask, COLOR_RGB2BGR);

    if (firstFrame.cols == 0) {
        mask.copyTo(firstFrame);
    }

    absdiff(mask, firstFrame, mask);
    cv::threshold(mask, mask, movement_threshold, 255, THRESH_BINARY);

    Moments m = moments(mask, true);
    Point centerOfMass(m.m10 / m.m00, m.m01 / m.m00);

    Mat output(input->rows, input->cols, CV_8UC3, Scalar(255, 255, 0));
    input->copyTo(output, mask);

    circle(output, centerOfMass, 5, Scalar(0, 0, 255), FILLED);
    namedWindow("Background Subtracted", WINDOW_AUTOSIZE);
    imshow("Background Subtracted", output);
}

bool HexposerImageProcessor::detectEdges() {
    // Edge detection
    Canny(src, dst, 50, 200, 3);

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    blur(src_gray, src_gray, Size(3, 3));


    // Copy edges to the images that will display the results in BGR
    // Probabilistic Line Transform
    cvtColor(dst, cdst, COLOR_GRAY2BGR);


    namedWindow("only edges", WINDOW_AUTOSIZE);
    imshow("only edges", cdst);

    return true;
}

void HexposerImageProcessor::detectLines() {
    int hexagonCount = 0;
    bool isHexagonCounted = false;
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 30, CV_PI / 180, line_sensitivity_threshold, min_line_length,
                max_line_gap); // runs the actual detection
    cdstP = cdst.clone();

    string print = "";
    // Draw the lines
    for (size_t j = 0; j < minRect.size(); j++) {

        Point2f rect_points[4];
        minRect[j].points(rect_points);

        float edge1 = euclideanDist(rect_points[0], rect_points[1]);
        float edge2 = euclideanDist(rect_points[1], rect_points[2]);
        float boxArea = edge1 * edge2;
        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            for (size_t i = 0; i < linesP.size(); i++) {
                Vec4i l = linesP[i];
                if (minRect[j].boundingRect().contains(Point(l[0], l[1]))) {
                    line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
                    if (!isHexagonCounted) {
                        hexagonCount++;
                        print.append(to_string(hexagonCount) + " Box size: " + to_string(boxArea) + " Edge1: " +
                                     to_string(edge1) + " Edge2: " + to_string(edge2));
                        if ((edge1 / unti_length > 1) xor (edge2 / unti_length > 1)) {
                            print.append(" Hex per Box: " + to_string(2) + "\n");
                        } else if ((edge1 / unti_length > 1) and (edge2 / unti_length > 1)) {
                            print.append(" Hex per Box: " + to_string(3) + "\n");
                        } else {
                            print.append(" Hex per Box: " + to_string(1) + "\n");
                        }
                        isHexagonCounted = true;
                    }
                }
            }
        }
        isHexagonCounted = false;
    }
    cout << print << endl;
    namedWindow("only edges", WINDOW_AUTOSIZE);
    imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
}

void HexposerImageProcessor::boundingBoxes_callback(int, void *) {
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

        float boxArea = (euclideanDist(rect_points[0], rect_points[1]) * euclideanDist(rect_points[2], rect_points[3]));

        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            for (int j = 0; j < 4; j++) {
                line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
            }
        }
    }
    namedWindow("Contours", WINDOW_AUTOSIZE);
    imshow("Contours", drawing);
}

HexposerImageProcessor::HexposerImageProcessor() {
    setVars();
    run();
}

HexposerImageProcessor::~HexposerImageProcessor() {
    delete this;
}

void HexposerImageProcessor::setVars(){
    box_sensitivity_threshold = 20;
    box_sensitivity_threshold_max = 255;
    movement_threshold = 0;
    min_line_length = 20;
    max_line_gap = 15;
    line_sensitivity_threshold = 10;
    hexagon_max_box_area = 10000;
    hexagon_min_box_area = 5500;
    unti_length = 80;
    frameRate = 26;
}
