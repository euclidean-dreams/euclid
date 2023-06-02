#pragma once

#include "paradigm.h"

namespace PROJECT_NAMESPACE {

struct Pixel {
    uint32_t color;

    Pixel(int red, int green, int blue) {
        uint8_t char_red = red;
        uint8_t char_green = green;
        uint8_t char_blue = blue;
        uint8_t char_alpha = 255;
        color = char_blue;
        color |= ((uint32_t) char_green << 8);
        color |= ((uint32_t) char_red << 16);
        color |= ((uint32_t) char_alpha << 24);
    }

    Pixel(int red, int green, int blue, int alpha) {
        uint8_t char_red = red;
        uint8_t char_green = green;
        uint8_t char_blue = blue;
        uint8_t char_alpha = alpha;
        color = char_blue;
        color |= ((uint32_t) char_green << 8);
        color |= ((uint32_t) char_red << 16);
        color |= ((uint32_t) char_alpha << 24);
    }
};

class Point {
public:
    float x;
    float y;
    float z;

    Point(float x, float y) :
            x{x}, y{y}, z{0} {}

    static Point from_polar(float radius, float theta) {
        return Point{radius * cos(theta), radius * sin(theta)};
    }
};

}
