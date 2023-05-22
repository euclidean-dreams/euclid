#pragma once

#include "foundation.h"

namespace PROJECT_NAMESPACE {

class Lumion : public Entity {
private:
    int fundamental;

public:
    Lumion(int fundamental_frequency) :
            fundamental{fundamental_frequency} {

    }

    float perceive(sp<Signal<float>> &signal) {
        float result = 0;

//        // perceive only fundamental
//        result += signal->get_sample(fundamental);

        // perceive all upper partials
        // needs more research, appears to highlight birdsong?
        auto cur_partial = fundamental;
        while (cur_partial < signal->size()) {
            result -= signal->get_sample(cur_partial);
            cur_partial += fundamental;
        }
        result = -result;

        return result;
    }
};

class Psyche : public Entity {
    int LOWEST_FUNDAMENTAL = 2;
    lst<Lumion> lumions;

public:
    Psyche(int lumion_count) : lumions{} {
        for (int fundamental = LOWEST_FUNDAMENTAL; fundamental < lumion_count; fundamental++) {
            lumions.emplace_back(fundamental);
        }
    }

    sp<Signal<float>> perceive(sp<Signal<float>> &signal) {
        auto result = mksp<Signal<float>>();
        for (int fundamental = 0; fundamental < LOWEST_FUNDAMENTAL; fundamental++) {
            result->push_back(0);
        }
        for (auto &lumion: lumions) {
            result->push_back(lumion.perceive(signal));
        }
        return result;
    }
};

}
