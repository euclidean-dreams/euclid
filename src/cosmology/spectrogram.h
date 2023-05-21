#pragma once

#include "foundation.h"
#include <list>

namespace PROJECT_NAMESPACE {

class Spectrogram : public Entity {
private:
    int width;
    int height;
    lst<sp<Signal<float>>> stft_frames;
    int view_index = 0;
    float color_gain = 1.2;

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

    void shift_view(int delta) {
        view_index += delta;
        auto stft_size = (*stft_frames.begin())->size();
        if (view_index + height > stft_size) view_index = stft_size - height;
        if (view_index < 0) view_index = 0;
    }

    void scale_color_gain(float delta) {
        color_gain += delta;
        if (color_gain < 0) color_gain = 0;
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
            for (int trimmed = 0; trimmed < view_index; trimmed++) {
                stft_iterator++;
            }
            auto stft_end_iterator = (*stft_frames_iterator)->end();
            for (int y = height - 1; y >= 0; y--) {
                int magnitude;
                if (stft_iterator == stft_end_iterator) {
                    magnitude = 100;
                } else {
                    magnitude = scast<int>(*stft_iterator) * color_gain;
                    stft_iterator++;
                }
                if (magnitude < 0) magnitude = 0;
                auto red = Pixel::trim(magnitude);
                auto green = Pixel::trim(magnitude * 3) / 3;
                auto blue = Pixel::trim(magnitude * 8) / 3;

                auto index = x + y * width;
                if (index < result->size()) {
                    result->set_sample(x + y * width, Pixel{red, green, blue});
                }

            }
            stft_frames_iterator++;
        }
        return result;
    }
};

}
