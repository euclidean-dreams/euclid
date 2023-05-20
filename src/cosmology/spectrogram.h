#pragma once

#include "foundation.h"
#include <list>

namespace PROJECT_NAMESPACE {

class Spectrogram : public Entity {
private:
    int width;
    int height;
    lst<sp<Signal<float>>> stft_frames;

public:
    Spectrogram(int width, int height)
            : width{width},
              height{height},
              stft_frames{} {
    }

    void process(sp<Signal<float>> stft) {
        stft_frames.push_back(mv(stft));
        while (stft_frames.size() > width) {
            stft_frames.pop_front();
        }
    }

    up<Signal<Pixel>> observe() {
        auto result = mkup<Signal<Pixel>>();
        result->populate(width * height, Pixel{0, 0, 0});
        auto stft_frames_iterator = stft_frames.begin();
        for (int x = 0; x < width; x++) {
            if (stft_frames_iterator == stft_frames.end()) {
                break;
            }
            auto stft_iterator = (*stft_frames_iterator)->begin();
            auto stft_end_iterator = (*stft_frames_iterator)->end();
            for (int y = height - 1; y >= 0; y--) {
                int stft_value;
                if (stft_iterator == stft_end_iterator) {
                    stft_value = 100;
                } else {
                    stft_value = scast<int>(*stft_iterator) * 10;
                    stft_iterator++;
                }
                if (stft_value > 255) stft_value = 255;
                if (stft_value < 0) stft_value = 0;

                auto index = x + y * width;
                if (index < result->size()) {
                    result->set_sample(x + y * width, Pixel{0, stft_value / 2, stft_value});
                }
            }
            stft_frames_iterator++;
        }
        return result;
    }
};

}
