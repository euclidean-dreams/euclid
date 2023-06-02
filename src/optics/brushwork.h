#pragma once

#include "axioms.h"
#include "cosmology/psyche.h"

namespace PROJECT_NAMESPACE {

float EXCITATION_THRESHOLD = 3;

class Canvas : public Name {
public:
    int width;
    int height;
    Signal<Pixel> pixels_dep;
    SDL_Texture *texture;
    Pixel *pixels;

    Canvas(int width, int height) :
            width{width},
            height{height},
            pixels_dep{},
            texture{},
            pixels{} {

        pixels_dep.populate(width * height, Pixel{0, 0, 0});
        texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                width,
                height
        );
        auto pitch = scast<int>(width * sizeof(Pixel));
        void * pixel_buffer;
        SDL_LockTexture(texture, nullptr, &pixel_buffer, &pitch);
        pixels = scast<Pixel *>(pixel_buffer);
    }

    void paint(Point point, Pixel color) {
        int x = point.x;
        int y = point.y;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[x + (height - (y + 1)) * width] = color;
        }
    }

    SDL_Texture *render() {
        SDL_UnlockTexture(texture);
        return texture;
    }
};

class Ray : public Name {
public:
    Luon &luon;
    float rotation;
    lst<Point> body;

    Ray(Luon &luon, Point origin, float rotation) :
            luon{luon},
            rotation{rotation},
            body{} {
        body.push_back(origin);
    }

    void paint_body(Canvas &canvas, Pixel color) {
        float displacement = abs(luon.delta / 3);
        while (displacement > 0) {
            auto step_distance = 0.7;
            displacement -= step_distance;
            if (luon.delta > 0) {
                rotation += luon.delta / 333;
                auto cur_tip = *(--body.end());
                auto new_position = Point::from_polar(step_distance + luon.delta / 33, rotation);
                body.emplace_back(cur_tip.x + new_position.x, cur_tip.y + new_position.y);
            } else {
                if (body.size() > 1)
                    body.pop_front();
            }
        }

        for (auto &point: body) {
            canvas.paint(point, color);
        }
    }
};

class Sunbeam : public Name {
private:
    int MAX_DEPTH = 1;

public:
    Luon &luon;
    Ray ray;
    float energy_requirement;
    int depth;
    lst<Sunbeam> branching_beams;
    int age = 0;

    Sunbeam(Luon &luon, Point origin, float rotation, float energy_requirement, int depth) :
            luon{luon},
            ray{luon, origin, rotation},
            energy_requirement{energy_requirement},
            depth{depth},
            branching_beams{} {
    }

    void draw(Canvas &canvas) {
        // color - luon energy
        auto color_mag = luon.energy;
        auto red = trim(color_mag * 9 + 44, 0, 255);
        auto green = trim(color_mag * 3 + 9, 0, 233);
        auto blue = trim(color_mag / 3, 0, 33);

        ray.paint_body(canvas, {red, green, blue});

        // replicate
        if (depth < MAX_DEPTH) {
            // trough detected
            if (luon.prior_delta <= 0 && luon.delta > 0) {
                auto origin = *(--ray.body.end());
                branching_beams.emplace_back(luon, origin, ray.rotation + 0.3, luon.energy, depth + 1);
            }
        }

        for (auto &beam: branching_beams) {
            beam.draw(canvas);
        }
    }

    bool live() {
        // return true to continue being alive
        if (luon.energy < energy_requirement)
            return false;

        // clean up branching beams
        auto cur_branch = branching_beams.begin();
        while (cur_branch != branching_beams.end()) {
            if (not cur_branch->live())
                cur_branch = branching_beams.erase(cur_branch);
            else
                cur_branch++;
        }

        age += 1;
        return true;
    }
};

class SunBeams : public Name {
private:
    int width;
    int height;
    sp<Psyche> psyche;
    lst<Sunbeam> beams;
    int age;

public:
    SunBeams(int width, int height, sp<Psyche> psyche)
            : width{width},
              height{height},
              psyche{psyche} {

    }

    SDL_Texture *observe() {
        age++;
        Canvas canvas{width, height};
        for (auto &luon: *psyche->luons) {
            if (luon.energy > EXCITATION_THRESHOLD) {
                Point origin{scast<float>(rand() % width), scast<float>(rand() % height)};
                float direction = 2 * M_PI * cos(age / 64);
                beams.emplace_back(luon, origin, direction, EXCITATION_THRESHOLD, 0);
            }
        }
        auto cur_stalk = beams.begin();
        while (cur_stalk != beams.end()) {
            if (not cur_stalk->live())
                cur_stalk = beams.erase(cur_stalk);
            else
                cur_stalk++;
        }
        for (auto &ray: beams) {
            ray.draw(canvas);
        }
        return canvas.render();
    }
};

}
