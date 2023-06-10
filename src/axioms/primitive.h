#pragma once

#include "paradigm.h"

namespace PROJECT_NAMESPACE {


struct Color {
public:
    int red;
    int green;
    int blue;
    int alpha;
};

class Point {
public:
    float x;
    float y;
    float z;

    Point(float x, float y) :
            x{x}, y{y}, z{0} {}

    static Point from_polar(Point origin, float radius, float theta) {
        return Point{origin.x + radius * cos(theta), origin.y + radius * sin(theta)};
    }
};

}
