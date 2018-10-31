#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
using namespace cv;
using namespace std;

// Declare the output variables
Mat dst, cdst, cdstP, src, src_gray, firstFrame, backGroundSubtracted;

int thresh = 100;
int max_thresh = 255;

int threshold_value = 0;
int minValue = 0;
int maxValue = 0;
int const max_value = 255;
RNG rng(12345);

const string maxDistance = "MaxDistance";
const string minLenght = "MinLenght";

const string srcWindowName = "Source";
const string stdHoughLineWindowName = "Detected Lines (in red) - Standard Hough Line Transform";
const string stdPropaLineWindowName = "Detected Lines (in red) - Probabilistic Line Transform";
int frameRate = 26;

void thresh_callback(int, void*);

cv::Mat processBackgroundSubtraction(const cv::Mat&input){
    Mat mask(input.rows, input.cols, CV_8UC1);
    cvtColor(input, mask, CV_RGB2GRAY);

    if (firstFrame.cols == 0){
        mask.copyTo(firstFrame);
    }

    absdiff(mask, firstFrame, mask);
    cv::threshold(mask, mask, threshold_value, 255, CV_THRESH_BINARY);

    Moments m = moments(mask,true);
    Point centerOfMass(m.m10/m.m00, m.m01/m.m00);


    Mat output(input.rows, input.cols, CV_8UC3, Scalar(255,255, 0));
    input.copyTo(output, mask);

    circle(output, centerOfMass, 5, Scalar(0, 0, 255), CV_FILLED);

    return output;

}

int main(int argc, char** argv)
{
    VideoCapture cap(1); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    namedWindow(srcWindowName, WINDOW_FREERATIO);
    createTrackbar(maxDistance, srcWindowName, &maxValue, max_value, nullptr);
    createTrackbar(minLenght, srcWindowName, &minValue, max_value, nullptr);
    createTrackbar("Threshold subtraction", "Source", &threshold_value, max_thresh, nullptr),
    createTrackbar( "Threshold:", "Source", &thresh, max_thresh, thresh_callback );

    for(;;){
        cap >> src;

        backGroundSubtracted = processBackgroundSubtraction(src);

        // Edge detection
        Canny(src, dst, 50, 200, 3);
        // Copy edges to the images that will display the results in BGR
        cvtColor(dst, cdst, COLOR_GRAY2BGR);
        cvtColor(src, src_gray, COLOR_BGR2GRAY);
        blur(src_gray, src_gray, Size(3,3));

        cdstP = cdst.clone();
        // Probabilistic Line Transform
        vector<Vec4i> linesP; // will hold the results of the detection
        HoughLinesP(dst, linesP, 1, CV_PI/180, 10, minValue, maxValue ); // runs the actual detection
        // Draw the lines
        for( size_t i = 0; i < linesP.size(); i++ )
        {
            Vec4i l = linesP[i];
            line( cdstP, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, LINE_AA);
        }
        // Show results
        imshow("Background Subtracted", backGroundSubtracted);
        imshow(srcWindowName, src);
        imshow(stdPropaLineWindowName, cdstP);
        thresh_callback(0,0);

        // Wait and Exit
        waitKey(frameRate);
    }

    return 0;
}

void thresh_callback(int, void* )
{
    Mat threshold_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    findContours( threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
    vector<RotatedRect> minRect( contours.size() );
    vector<RotatedRect> minEllipse( contours.size() );
    for( size_t i = 0; i < contours.size(); i++ )
    { minRect[i] = minAreaRect( contours[i] );
        if( contours[i].size() > 5 )
        { minEllipse[i] = fitEllipse( contours[i] ); }
    }
    Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        // contour
        drawContours( drawing, contours, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
        // ellipse
        ellipse( drawing, minEllipse[i], color, 2, 8 );
        // rotated rectangle
        Point2f rect_points[4]; minRect[i].points( rect_points );
        for( int j = 0; j < 4; j++ )
            line( drawing, rect_points[j], rect_points[(j+1)%4], color, 1, 8 );
    }
    namedWindow( "Contours", WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
}
