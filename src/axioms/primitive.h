#pragma once

#include "axioms.h"

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

    static int trim(int value) {
        trim(value, 0, 255);
        return value;
    }

    static int trim(int value, int lower_bound, int upper_bound) {
        if (value > upper_bound) value = upper_bound;
        if (value < lower_bound) value = lower_bound;
        return value;
    }
};

}
