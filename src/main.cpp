#include <iostream>
#include <string>
#include <fstream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "ImageProcessor.cpp"
#include "nlohmann/json.hpp"

int main(int argc, char **argv) {
    ImageProcessor imageProcessor = ImageProcessor();
    imageProcessor.run();
    return 0;
}