cmake_minimum_required(VERSION 3.12)
project(hexaposer)

set(CMAKE_CXX_STANDARD 17)

# Where to find CMake modules and OpenCV
set(OpenCV_DIR "C:\\opencv\\opencv344\\build86")
set(nlohmann_json_DIR "C:\\Users\\wnabo\\Desktop\\json\\build")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
 
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(hextractor src/main.cpp src/Util.cpp src/Util.h src/Hexagon.cpp src/Hexagon.h)

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)

# linking
target_link_libraries(hextractor ${OpenCV_LIBS} nlohmann_json::nlohmann_json)