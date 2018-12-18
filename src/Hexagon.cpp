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

Hexagon::Hexagon(int x, int y, const std::string &color) : x(x), y(y), color(color) {
    this->color = color;
    this->x = x;
    this->y = y;
}

nlohmann::json Hexagon::toJSON() {
    nlohmann::json hex = {
                    {"color", color},
                    {"posZ", x},
                    {"posY", y}
    };
    return hex;
}


