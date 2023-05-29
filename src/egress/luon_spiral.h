#pragma once

#include "foundation.h"
#include <list>

namespace PROJECT_NAMESPACE {

class LuonSpiral : public Name {
private:
    int width;
    int height;
    sp<Psyche> psyche;
    vec<int> x_values;
    vec<int> y_values;

public:
    LuonSpiral(int width, int height, sp<Psyche> psyche)
            : width{width},
              height{height},
              psyche{psyche} {
        auto index = 0;
        for (auto &luon: *psyche->luons) {
            // location - spiral function
            cmplx radius(0.3, 0);
            if (index > psyche->luons->size() / 4) {
                radius = {0.1, 0};
            }
            cmplx spacing(0.0001, 0);
            cmplx t(index, 0);
            const cmplx i(0, 1);
            auto point = radius * t * exp(-2 * M_PI * i * pow(t, 2) * spacing);
            int x = scast<float>(point.real()) + scast<float>(width) / 2;
            int y = scast<float>(point.imag()) + scast<float>(height) / 2;

            x_values.push_back(x);
            y_values.push_back(y);
            index++;
        }
    }

    SDL_Texture *observe() {
        auto signal = mkup<Signal<Pixel>>();
        signal->populate(width * height, Pixel{0, 0, 0});

        int index = 0;
        for (auto &luon: *psyche->luons) {
            // color - luon energy
            auto color_magnitude = luon.current_energy;
            auto red = 255 - Pixel::trim(color_magnitude - 70);
            auto green = 255 - Pixel::trim(color_magnitude - 20);
            auto blue = 255 - Pixel::trim(color_magnitude);

            // scale
            int scale = luon.current_energy;
            int initial_x = x_values[index];
            int initial_y = y_values[index];
            for (int x = initial_x; x < initial_x + scale && x < width; x++) {
                for (int y = initial_y; y < initial_y + scale && y < height; y++) {
                    signal->set_sample(x + y * width, Pixel{red, green, blue});
                }
            }
            index++;
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
