#pragma once

#include "paradigm.h"
#include "cosmology/psyche.h"
#include "opus.h"

using namespace std::complex_literals;

namespace PROJECT_NAMESPACE {


class Flower : public Name {
public:
    Luon &luon;
    Point origin;

    Flower(Luon &luon, Point origin) : luon{luon}, origin{origin} {

    }

    void observe(Canvas &canvas) {
        auto alpha_mag = luon.energy / 50;
        auto alpha = embind(240, 255 - alpha_mag, 255);

        for (int i = 0; i < 4; i++) {
            auto red = embind(128, 128 + i * 32, 255);
            auto green = embind(64, 128 + i * 32 - 64, 200);
            auto blue = embind(0, 0, 0);

            SDL_Rect area_to_paint;
            area_to_paint.x = origin.x;
            area_to_paint.y = origin.y;
            area_to_paint.w = embind(0, luon.energy / 2 - i * 33, 333);
            area_to_paint.h = embind(0, luon.energy / 2 - i * 33, 333);
            canvas.paint_rect(area_to_paint, {red, green, blue, alpha});
        }
    }
};

class Stalk : public Name {
public:
    float age = 0;
    lst<Flower> flowers;

    void observe(Harmony &harmony, Canvas &canvas) {
        float t = 0;
        int incr_luon = 0;
        auto granularity = 0.07;
        while (incr_luon < harmony.luons->size()) {
            auto &cur_luon = (*harmony.luons)[incr_luon];
            auto luon_energy = cur_luon->energy;
            if (luon_energy > 3) {
                auto spread = embind_flt(0, luon_energy / 200 - scflt(incr_luon) / harmony.luons->size() / 10, 0.4);
                auto complex_result = scast<cmplx>(-t * 3) * exp(std::pow(t, 0.7) * 2i + M_PI * scast<cmplx>(spread));
                float y = complex_result.real() * 3;
                float x = complex_result.imag() * 3;
                x += scflt(render_width) / 2;
                y += scflt(render_height) / 2;

                auto red = embind(0, luon_energy / 3, 128);
                auto green = embind(0, luon_energy * 9 + 9, 233);
                auto blue = embind(0, luon_energy * 27 + 44, 255);
                auto alpha = embind(128, luon_energy * 3, 255);

                auto scale = embind(0, luon_energy / 2 + 1, 128);
                SDL_Rect area_to_paint;
                area_to_paint.x = x - scale / 2;
                area_to_paint.y = y - scale / 2;
                area_to_paint.w = scale;
                area_to_paint.h = scale;
                canvas.paint_rect(area_to_paint, {red, green, blue, alpha});

                if (spread > 0.4) {
                    flowers.emplace_back(*cur_luon, Point{x, y});
                }
                while (flowers.size() > 10) {
                    flowers.pop_front();
                }
                for (auto &flower: flowers) {
                    flower.observe(canvas);
                }
            }
            t += granularity;
            incr_luon++;
        }
        age += 5;
    }
};

class Sage : public Name {
private:
    int width;
    int height;
    up<Harmony> harmony;

public:
    Sage(int width, int height, up<Harmony> harmony)
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
