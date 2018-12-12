#include <iostream>
#include <string>
#include <fstream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "ImageProcessor.cpp"
#include "nlohmann/json.hpp"

ImageProcessor *imageProcessor;

void thread_function() {
    *imageProcessor = ImageProcessor();
    imageProcessor->run();
}
int main(int argc, char **argv) {


    std::thread threadObj(thread_function);

    std::vector<Hexagon> hexas;
    while (true) {
    }
    return 0;
}