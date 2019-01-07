//
// Created by wnabo on 11.12.2018.
//

#include "Hexagon.h"

int Hexagon::getX() const {
    return x;
}

void Hexagon::setX(int x) {
    Hexagon::x = x;
}

int Hexagon::getY() const {
    return y;
}

void Hexagon::setY(int y) {
    Hexagon::y = y;
}

const std::string &Hexagon::getColor() const {
    return color;
}

void Hexagon::setColor(const std::string &color) {
    Hexagon::color = color;
}

Hexagon::Hexagon() {}

nlohmann::json Hexagon::toJSON() {
    nlohmann::json hex = {
                    {"color", color},
                    {"posY", y},
                    {"posZ", x}

    };
    return hex;
}

cv::Point2i Hexagon::calculateGridPosition(cv::Point2i point, cv::Point2i gridPoint, int hexagon_length) {
    gridPoint.x = point.x * 1 / hexagon_length;
    gridPoint.y = (point.y - 1 / 2 * point.x) * 1 / hexagon_length;
    return gridPoint;
}


