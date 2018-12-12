//
// Created by wnabo on 11.12.2018.
//

#ifndef HEXAPOSER_HEXAGON_H
#define HEXAPOSER_HEXAGON_H


#include <string>
#include <nlohmann/json.hpp>

class Hexagon {
public:
    Hexagon(int x, int y, const std::string &color);

    int getX() const;

    void setX(int x);

    int getY() const;

    void setY(int y);

    const std::string &getColor() const;

    void setColor(const std::string &color);

    nlohmann::json toJSON();

private:
    int x;
    int y;
    std::string color;
};


#endif //HEXAPOSER_HEXAGON_H
