#include <c++/iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "ImageProcessor.cpp"

ImageProcessor *imageProcessor;

void thread_function()
{
    *imageProcessor = ImageProcessor();
    imageProcessor->run();
}


int main(int argc, char **argv) {

    std::thread threadObj(thread_function);

    while(true){

        cout << ImageProcessor::rectanglesCount << endl;
    }
    return 0;
}