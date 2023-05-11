#pragma once

#include "foundation.h"
#include <list>

namespace PROJECT_NAMESPACE {

class Spectrogram : NonCopyable {
private:
    int width;
    int height;
    lst<sp<Signal<float>>> stft_frames;
    lst<sp<Signal<float>>> harmony_frames;

public:
    Spectrogram(int width, int height)
            : width{width},
              height{height},
              stft_frames{},
              harmony_frames{} {
    }

    void process(sp<Signal<float>> stft, up<Signal<float>> harmony) {
        stft_frames.push_back(mv(stft));
        while (stft_frames.size() > width) {
            stft_frames.pop_front();
        }

        harmony_frames.push_back(mv(harmony));
        while (harmony_frames.size() > width) {
            harmony_frames.pop_front();
        }
    }

    up<Signal<Pixel>> observe() {
        auto result = mkup<Signal<Pixel>>();
        result->populate(width * height, Pixel{0, 0, 0});
        auto stft_frames_iterator = stft_frames.begin();
        auto harmonic_frames_iterator = harmony_frames.begin();
        for (int x = 0; x < width; x++) {
            if (stft_frames_iterator == stft_frames.end() or harmonic_frames_iterator == harmony_frames.end()) {
                break;
            }
            auto stft_iterator = (*stft_frames_iterator)->begin();
            auto stft_end_iterator = (*stft_frames_iterator)->end();
            auto harmonic_iterator = (*harmonic_frames_iterator)->begin();
            auto harmonic_end_iterator = (*harmonic_frames_iterator)->end();
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
                int harmonic_value = 0;
                if (harmonic_iterator == harmonic_end_iterator) {
                    harmonic_value = 20;
                } else {
                    harmonic_value = scast<int>(*harmonic_iterator) * 1000;
                    harmonic_iterator++;
                }
                if (harmonic_value > 255) harmonic_value = 255;
                if (harmonic_value < 0) harmonic_value = 0;

                auto index = x + y * width;
                if (index < result->size()) {

                    result->set_sample(x + y * width, Pixel{harmonic_value, 0, stft_value});
                }
            }
            stft_frames_iterator++;
            harmonic_frames_iterator++;
        }
        return result;
    }
};

}
