#pragma once

#include "axioms.h"

namespace PROJECT_NAMESPACE {

class Point {
public:
    float x;
    float y;
    float z;

    Point(float x, float y, float z) :
            x{x}, y{y}, z{z} {}

    static Point from_polar(float radius, float theta) {
        return Point{radius * cos(theta), radius * sin(theta), 0};
    }
};

}
