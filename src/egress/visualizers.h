#pragma once

#include "foundation.h"
#include <list>

namespace PROJECT_NAMESPACE {

class LuonSpiral : public Name {
private:
    sp<Psyche> psyche;
    vec<int> x_values;
    vec<int> y_values;

public:
    LuonSpiral(sp<Psyche> psyche) : psyche{psyche} {
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
            int x = scast<float>(point.real()) + scast<float>(render_width) / 2;
            int y = scast<float>(point.imag()) + scast<float>(render_height) / 2;

            x_values.push_back(x);
            y_values.push_back(y);
            index++;
        }
    }

    up<Signal<Pixel>> observe() {
        auto output = mkup<Signal<Pixel>>();
        output->populate(render_width * render_height, Pixel{0, 0, 0});

        int index = 0;
        for (auto &luon: *psyche->luons) {
            // color - luon energy
            auto color_magnitude = luon.current_energy * 10;
            auto red = 255 - Pixel::trim(color_magnitude - 70);
            auto green = 255 - Pixel::trim(color_magnitude - 20);
            auto blue = 255 - Pixel::trim(color_magnitude);

            // scale
            int scale = 5;
            int initial_x = x_values[index];
            int initial_y = y_values[index];
            for(int x = initial_x; x < scale; x++) {
                for(int y = initial_y; y < scale; y++) {
                    output->set_sample(x + y * render_width, Pixel{red, green, blue});
                }
            }
            index++;
        }
        return output;
    }

};

}
