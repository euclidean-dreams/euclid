#pragma once

#include "axioms.h"
#include "cosmology/psyche.h"
#include "opus.h"

using namespace std::complex_literals;

namespace PROJECT_NAMESPACE {

class Stalk : public Name {
public:
    float age = 0;

    void observe(Harmony &harmony, Canvas &canvas) {
        float t = 0;
        int incr_luon = 0;
        auto granularity = 0.07;
        while (incr_luon < harmony.luons->size()) {
            auto luon_energy = (*harmony.luons)[incr_luon]->energy;
            if (luon_energy > 3) {
                auto complex_result =
                        scast<cmplx>(-t * 3) * exp(std::pow(t, 0.7) * 2i + M_PI * scast<cmplx>(luon_energy / 333));
                float y = complex_result.real() * 3;
                float x = complex_result.imag() * 3;
                x += scflt(render_width) / 2;
                y += scflt(render_height) / 2;

                auto red = trim(luon_energy / 3, 0, 128);
                auto green = trim(luon_energy * 9 + 9, 0, 233);
                auto blue = trim(luon_energy * 27 + 44, 0, 255);

                auto scale = luon_energy / 3 + 1;
                SDL_Rect area_to_paint;
                area_to_paint.x = x - scale / 2;
                area_to_paint.y = y - scale / 2;
                area_to_paint.w = scale;
                area_to_paint.h = scale;
                canvas.paint_rect(area_to_paint, {red, green, blue});
            }
            t += granularity;
            incr_luon++;
        }
        age += granularity;
    }
};

class Aspect : public Name {
private:
    int width;
    int height;
    up<Harmony> harmony;

public:
    Aspect(int width, int height, up<Harmony> harmony)
            : width{width},
              height{height},
              harmony{mv(harmony)} {

    }

    SDL_Texture *observe() {
        Canvas canvas{width, height};
        Stalk stalk;
        stalk.observe(*harmony, canvas);
        return canvas.finalize();
    }
};

}
