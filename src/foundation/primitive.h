#pragma once

#include "foundation.h"

namespace PROJECT_NAMESPACE {

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;

    uint32_t serialize() const {
        uint32_t result = red;
        result |= ((uint32_t) green << 8);
        result |= ((uint32_t) blue << 16);
        result |= ((uint32_t) alpha << 24);
        return result;
    }
};

}
