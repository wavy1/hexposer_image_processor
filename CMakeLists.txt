cmake_minimum_required(VERSION 3.12)
project(hexaposer)

set(CMAKE_CXX_STANDARD 17)

# Where to find CMake modules and OpenCV
set(OpenCV_DIR C:\\openCV\\build4mingw32\\install)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOIC ON)
set(CMAKE_PREFIX_PATH C:\\Qt\\5.11.2\\mingw53_32\\lib\\cmake)

find_package(OpenCV CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Network CONFIG REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${Qt5_INCLUDE_DIRS})

add_executable(hexaposer src/main.cpp src/HexposerImageProcessor.h src/HexposerImageProcessor.cpp src/HexposerServerCommunicator.h src/HexposerServerCommunicator.cpp src/Client.cpp src/Client.h)

# add libs you need
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs)
set(Qt5_LIBS Qt5::Core Qt5::Widgets Qt5::Network)

# linking
target_link_libraries(hexaposer ${OpenCV_LIBS} ${Qt5_LIBS})

