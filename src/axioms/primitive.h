#pragma once

#include "paradigm.h"

namespace PROJECT_NAMESPACE {


struct Color {
public:
    int red;
    int green;
    int blue;

    static Color gradiate(Color first_color, Color second_color, float scale) {
        if (scale > 1)
            scale = 1;
        auto res_red = scale * scast<float>(second_color.red - first_color.red) + first_color.red;
        auto res_green = scale * scast<float>(second_color.green - first_color.green) + first_color.green;
        auto res_blue = scale * scast<float>(second_color.blue - first_color.blue) + first_color.blue;
        return {scast<int>(res_red), scast<int>(res_green), scast<int>(res_blue)};
    }
};

struct Pixel {
    uint32_t color;

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

    Pixel(int red, int green, int blue) :
            Pixel{red, green, blue, 255} {
    }

    Pixel(Color color) :
            Pixel{color.red % 256, color.green % 256, color.blue % 256} {
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
