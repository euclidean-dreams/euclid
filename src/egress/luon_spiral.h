#pragma once

#include "foundation.h"
#include <list>

namespace PROJECT_NAMESPACE {

class LuonSpiral : public Name {
private:
    int width;
    int height;
    sp<Psyche> psyche;
    vec<Point> luon_positions;

public:
    LuonSpiral(int width, int height, sp<Psyche> psyche)
            : width{width},
              height{height},
              psyche{psyche} {
        for (auto &luon: *psyche->luons) {
            auto luon_count = psyche->luons->size();
            float theta = luon.fundamental;
            float radius = cos(123.0 * luon.fundamental / luon_count + M_PI) * width;
            auto point = Point::from_polar(radius, theta);

            // shift to center of screen
            point.x += width / 2;
            point.y += height / 2;
            luon_positions.push_back(point);
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
            auto initial = luon_positions[index];
            for (int x = initial.x; x < initial.x + scale; x++) {
                for (int y = initial.y; y < initial.y + scale; y++) {
                    auto signal_index = x + y * width;
                    if (signal_index < signal->size()) {
                        signal->set_sample(signal_index, Pixel{red, green, blue});
                    }
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
