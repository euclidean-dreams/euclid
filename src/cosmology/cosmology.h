#pragma once

#include <complex>
#include "foundation.h"
#include "opus.h"

namespace PROJECT_NAMESPACE {

class Cosmology : NonCopyable {
private:
    int width;
    int height;
    lst<up<Signal<cmplx>>> frames;

public:
    Cosmology(int width, int height)
            : width{width},
              height{height},
              frames{} {
    }

    void process(up<Signal<cmplx>> stft) {
        frames.push_back(mv(stft));
        while (frames.size() > 1) {
            frames.pop_front();
        }
    }

    up<Signal<Pixel>> observe() {
        auto result = mkup<Signal<Pixel>>();
        result->populate(width * height, Pixel{0, 0, 0});
        if (!frames.empty()) {
            int y = height;
            for (auto &sample: *frames.back()) {
                auto magnitude = scast<float>(std::sqrt(std::pow(sample.real(), 2) + std::pow(sample.imag(), 2)));
                int value = scast<int>(magnitude) * 10;
                if (value > 255) {
                    value = 255;
                }
                if (value < 0) {
                    value = 0;
                }
                for (int x = 0; x < width; x++) {
                    auto index = x + y * width;
                    if (index < result->size()) {
                        result->set_sample(index, Pixel{10, value / 2, value});
                    }
                }
                y--;
            }
        }
        return result;
    }
};

}
