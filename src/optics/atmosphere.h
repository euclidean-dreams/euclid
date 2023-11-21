#pragma once

#include "paradigm.h"
#include "cosmology/psyche.h"
#include "opus.h"

namespace PROJECT_NAMESPACE {


class Cloud : public Name {
public:
    Luon &luon;
    Point origin;

    Cloud(Luon &luon, Point origin) : luon{luon}, origin{origin} {

    }

    void observe(Canvas &canvas) {
        auto color_mag = luon.energy / 50;
        auto alpha = embind(240, 255 - color_mag, 255);

        SDL_Rect area_to_paint;
        area_to_paint.x = origin.x;
        origin.x += rand() % 5 - 3;
        origin.x = cyclic_embind(0, origin.x, canvas.width);
        area_to_paint.y = origin.y;
        origin.y += rand() % 5 - 3;
        origin.y = cyclic_embind(0, origin.y, canvas.height);
        area_to_paint.w = embind(0, luon.energy / 2, 333);
        area_to_paint.h = embind(0, luon.energy / 2, 333);
        canvas.paint_rect(area_to_paint, {255, 255, 255, alpha});
    }

};

class Atmosphere : public Name {
private:
    int width;
    int height;
    up<Harmony> harmony;
    lst<Cloud> clouds;

public:
    Atmosphere(int width, int height, up<Harmony> harmony)
            : width{width},
              height{height},
              harmony{mv(harmony)} {
        for (auto &luon: *this->harmony->luons) {
            float x = rand() % width;
            float y = rand() % height;
            clouds.emplace_back(*luon, Point{x, y});
        }
    }

    SDL_Texture *observe() {
        Canvas canvas{width, height};
        canvas.paint_rect(canvas.area, {255, 255, 255, 255});
        for (auto &cloud: clouds) {
            cloud.observe(canvas);
        }
        return canvas.finalize();
    }
};

}
