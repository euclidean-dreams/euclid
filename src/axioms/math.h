#pragma once

#include "paradigm.h"

namespace PROJECT_NAMESPACE {

static int trim(int value, int lower_bound, int upper_bound) {
    if (value > upper_bound) value = upper_bound;
    if (value < lower_bound) value = lower_bound;
    return value;
}

}
