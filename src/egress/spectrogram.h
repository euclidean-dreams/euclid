#pragma once

#include "foundation.h"

namespace PROJECT_NAMESPACE {

class Spectrogram : public Name {
private:
    int width;
    int height;
    lst<sp<Signal<float>>> signals;
    int view_index = 0;

public:
    Spectrogram(int width, int height)
            : width{width},
              height{height},
              signals{} {
        while (signals.size() < width) {
            auto dummy_signal = mkup<Signal<float>>();
            while (dummy_signal->size() < height) {
                dummy_signal->push_back(0);
            }
            signals.push_back(mv(dummy_signal));
        }
    }

    void process(sp<Signal<float>> signal) {
        signals.push_back(mv(signal));
        while (signals.size() > width) {
            signals.pop_front();
        }
    }

    void shift_view(int delta) {
        view_index += delta;
        auto size = (*signals.begin())->size();
        if (view_index + height > size) view_index = size - height;
        if (view_index < 0) view_index = 0;
    }

    SDL_Texture *observe() {
        auto signal = mkup<Signal<Pixel>>();
        signal->populate(width * height, Pixel{0, 0, 0});
        auto cur_signal = signals.begin();
        for (int x = 0; x < width; x++) {
            if (cur_signal == signals.end()) {
                break;
            }
            auto cur_value = (*cur_signal)->begin();
            for (int trimmed = 0; trimmed < view_index; trimmed++) {
                cur_value++;
            }
            auto cur_value_end = (*cur_signal)->end();
            for (int y = height - 1; y >= 0; y--) {
                int magnitude;
                if (cur_value == cur_value_end) {
                    magnitude = 100;
                } else {
                    magnitude = scast<int>(*cur_value);
                    cur_value++;
                }
                if (magnitude < 0) magnitude = 0;
                auto red = Pixel::trim(magnitude);
                auto green = Pixel::trim(magnitude * 3) / 3;
                auto blue = Pixel::trim(magnitude * 8) / 3;

                auto index = x + y * width;
                if (index < signal->size()) {
                    signal->set_sample(x + y * width, Pixel{red, green, blue});
                }
            }
            cur_signal++;
        }

        void *pixel_buffer;
        int pitch = scast<int>(width * sizeof(Pixel));
        auto texture = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height
        );
        SDL_LockTexture(texture, nullptr, &pixel_buffer, &pitch);
        memcpy(pixel_buffer, signal->get_data_pointer(), signal->size() * sizeof(Pixel));
        SDL_UnlockTexture(texture);
        return texture;
    }
};

}
