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

/**
 * Casts the Hexagon in a json notation hexagon
 * @return json hexagon
 */
nlohmann::json Hexagon::toJSON() {
    nlohmann::json hex = {
                    {"color", color},
                    {"posY", y},
                    {"posZ", x}

    };
    return hex;
}

/**
 * Mapping function to calculate the gridPosition from the screenPosition
 * @param screenPosition : Position on the Screen, consists of x and y component
 * @param gridPosition : Position on the Grid, consist of x and y component
 * @param hexagon_length : Length of a hexagon
 * @return gridPosition
 */
cv::Point2i Hexagon::mapScreenToGridPosition(cv::Point2i screenPosition, cv::Point2i gridPosition, int hexagon_length) {
    gridPosition.x = screenPosition.x * 1 / hexagon_length;
    gridPosition.y = (screenPosition.y - 1 / 2 * screenPosition.x) * 1 / hexagon_length;
    return gridPosition;
}


