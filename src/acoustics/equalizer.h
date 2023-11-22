#pragma once

#include "paradigm.h"

namespace PROJECT_NAMESPACE {

class Equalizer : public Name {
private:
    float gain = 1.0;

public:
    void nudge_gain(float delta) {
        gain += delta;
        if (gain < 0) gain = 0;
    }

    up<Signal<float>> equalize(up<Signal<float>> origin) {
        auto output = mkup<Signal<float>>();
        for (auto &sample: *origin) {
            output->push_back(sample * gain);
        }
        return output;
    }
};

}
