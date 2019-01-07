//
// Created by wnabo on 11.12.2018.
//

#ifndef HEXAPOSER_HEXAGON_H
#define HEXAPOSER_HEXAGON_H


#include <string>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>

class Hexagon {
public:
    Hexagon();
    int getX() const;
    void setX(int x);
    int getY() const;
    void setY(int y);
    const std::string &getColor() const;
    void setColor(const std::string &color);
    nlohmann::json toJSON();
    cv::Point2i calculateGridPosition(cv::Point2i point, cv::Point2i gridPoint, int hexagon_length);

private:
    int x;
    int y;
    std::string color;
};


#endif //HEXAPOSER_HEXAGON_H
