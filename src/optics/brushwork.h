#pragma once

#include "axioms.h"
#include "cosmology/psyche.h"

namespace PROJECT_NAMESPACE {

class Canvas : public Name {
public:
    int width;
    int height;
    SDL_Texture *texture;
    Pixel *pixels;

    Canvas(int width, int height) :
            width{width},
            height{height},
            texture{},
            pixels{} {

        texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height
        );
        auto pitch = scast<int>(width * sizeof(Pixel));
        void *pixel_buffer;
        SDL_LockTexture(texture, nullptr, &pixel_buffer, &pitch);
        pixels = scast<Pixel *>(pixel_buffer);
        for (int i = 0; i < width * height; i++) {
            pixels[i] = {0, 0, 0};
        }
    }

    void paint(Point point, Pixel color) {
        int x = point.x;
        int y = point.y;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[x + (height - (y + 1)) * width] = color;
        }
    }

    SDL_Texture *finalize() {
        SDL_UnlockTexture(texture);
        return texture;
    }
};

class Brush : public Name {
public:
    Luon &luon;
    Point origin;
    float rotation;

    Brush(Luon &luon, Point origin, float rotation) :
            luon{luon},
            origin{origin},
            rotation{rotation} {
    }

    void paint(Canvas &canvas) {
        auto magnitude = luon.energy;
        int red = trim(magnitude / 3, 0, 255);
        int green = trim(magnitude / 10, 0, 255);
        int blue = trim(magnitude, 0, 255);
        auto scale = magnitude;
        if (scale > canvas.width / 100) {
            scale = canvas.width / 100;
        }
        for (int x_disp = 0; x_disp < scale; x_disp++) {
            for (int y_disp = 0; y_disp < scale; y_disp++) {
                float x = x_disp + origin.x;
                float y = y_disp + origin.y;
                canvas.paint({x, y}, {red, green, blue});
            }
        }
    }
};

class Entity : public Name {
private:
    int MAX_DEPTH = 1;

public:
    Luon &luon;
    float energy_requirement;
    int depth;
    lst<Brush> brushes;
    int age = 0;

    Entity(Luon &luon, Point origin, float rotation, float energy_requirement, int depth) :
            luon{luon},
            energy_requirement{energy_requirement},
            depth{depth},
            brushes{} {
        brushes.emplace_back(luon, origin, rotation);
    }

    void paint(Canvas &canvas) {
        for (auto &brush: brushes) {
            brush.paint(canvas);
        }
    }

    bool live() {
        // return true to continue being alive
        if (luon.energy < energy_requirement)
            return false;
        age += 1;
        return true;
    }

};

class Brushwork : public Name {
private:
    int width;
    int height;
    up<Harmony> harmony;
    lst<Entity> entities;
    int age = 0;

public:
    Brushwork(int width, int height, up<Harmony> harmony)
            : width{width},
              height{height},
              harmony{mv(harmony)} {

    }

    SDL_Texture *observe() {
        age++;
        Canvas canvas{width, height};
        for (auto &luon: *harmony->luons) {
            float excitation_threshold = 3;
            if (luon->energy > excitation_threshold) {
                Point origin{scast<float>(rand() % width), scast<float>(rand() % height)};
                float direction = 2 * M_PI * cos(age / 64);
                entities.emplace_back(*luon, origin, direction, excitation_threshold, 0);
            }
        }
        auto cur = entities.begin();
        while (cur != entities.end()) {
            if (not cur->live())
                cur = entities.erase(cur);
            else
                cur++;
        }
        while (entities.size() > 10000)
            entities.pop_front();
        for (auto &entity: entities) {
            entity.paint(canvas);
        }
        return canvas.finalize();
    }
};

}
