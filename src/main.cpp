#include <c++/4.8.3/iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

// Declare the output variables
cv::Mat dst, cdst, cdstP, src, src_gray, firstFrame;

const int cameraNo = 1;
int box_sensitivity_threshold = 20;
int box_sensitivity_threshold_max = 255;
int movement_threshold = 0;
int min_line_length = 20;
int max_line_gap = 15;
int line_sensitivity_threshold = 10;
int hexagon_max_box_area = 10000;
int hexagon_min_box_area = 5500;

int const sensitivity_max = 255;
int const hexagon_area_slider_max_ = 100000;

vector<RotatedRect> minRect;

const string srcWindowName = "Source Window";
int frameRate = 26;

void boundingBoxes_callback(int, void *);

void setupGUI();

void detectLines();

bool detectEdges();

void backgroundSubtraction_callback(int, void *);

float euclideanDist(cv::Point2f &p, cv::Point2f &q);


int main(int argc, char **argv) {
    VideoCapture cap(cameraNo); // open the default camera
    if (!cap.isOpened())  // check if we succeeded
        return -1;
    setupGUI();

    for (;;) {
        cap >> src;
        // Show results
        imshow(srcWindowName, src);
        if(detectEdges()){
            detectLines();
        }
        backgroundSubtraction_callback(movement_threshold, 0);
        boundingBoxes_callback(box_sensitivity_threshold, 0);

        // Wait and Exit
        waitKey(frameRate);
    }
}

float euclideanDist(cv::Point2f &p, cv::Point2f &q) {
    Point diff = p - q;
    return (float) cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}

void setupGUI() {
    namedWindow(srcWindowName, WINDOW_FREERATIO);
    createTrackbar("Line Sensitivity Threshold", srcWindowName, &line_sensitivity_threshold, sensitivity_max, nullptr);
    createTrackbar("Max Line Gap", srcWindowName, &max_line_gap, sensitivity_max, nullptr);
    createTrackbar("Min Line Length", srcWindowName, &min_line_length, sensitivity_max, nullptr);
    createTrackbar("Threshold subtraction:", srcWindowName, &movement_threshold, box_sensitivity_threshold_max,
                   backgroundSubtraction_callback);
    createTrackbar("Box Sensitivity:", srcWindowName, &box_sensitivity_threshold, box_sensitivity_threshold_max,
                   boundingBoxes_callback);
    createTrackbar("Area Max", srcWindowName, &hexagon_max_box_area, hexagon_area_slider_max_, boundingBoxes_callback);
    createTrackbar("Area Min", srcWindowName, &hexagon_min_box_area, hexagon_area_slider_max_, boundingBoxes_callback);

}

void backgroundSubtraction_callback(int, void *) {
    Mat* input = &src;
    Mat mask(input->rows, input->cols, CV_8UC1);
    cvtColor(*input, mask, CV_RGB2GRAY);

    if (firstFrame.cols == 0) {
        mask.copyTo(firstFrame);
    }

    absdiff(mask, firstFrame, mask);
    cv::threshold(mask, mask, movement_threshold, 255, CV_THRESH_BINARY);

    Moments m = moments(mask, true);
    Point centerOfMass(m.m10 / m.m00, m.m01 / m.m00);

    Mat output(input->rows, input->cols, CV_8UC3, Scalar(255, 255, 0));
    input->copyTo(output, mask);

    circle(output, centerOfMass, 5, Scalar(0, 0, 255), CV_FILLED);
    namedWindow("Background Subtracted", WINDOW_AUTOSIZE);
    imshow("Background Subtracted", output);
}

bool detectEdges() {
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

void detectLines() {
    int hexagonCount = 0;
    bool isHexagonCounted = false;
    vector<Vec4i> linesP; // will hold the results of the detection
    HoughLinesP(dst, linesP, 1, CV_PI / 180, line_sensitivity_threshold, min_line_length,
                max_line_gap); // runs the actual detection
    cdstP = cdst.clone();

    // Draw the lines
    for (size_t j = 0; j < minRect.size(); j++) {

        Point2f rect_points[4];
        minRect[j].points(rect_points);

        float boxArea = (euclideanDist(rect_points[0], rect_points[1]) * euclideanDist(rect_points[2], rect_points[3]));
        if (boxArea < hexagon_max_box_area && boxArea > hexagon_min_box_area) {
            for (size_t i = 0; i < linesP.size(); i++) {
                Vec4i l = linesP[i];
                if (minRect[j].boundingRect().contains(Point(l[0], l[1]))) {
                    line(cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);
                    if(!isHexagonCounted){
                        hexagonCount++;
                        isHexagonCounted = true;
                    }
                }
            }
        }
        isHexagonCounted = false;
    }
    cout << hexagonCount << endl;
    namedWindow("only edges", WINDOW_AUTOSIZE);
    imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP);
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