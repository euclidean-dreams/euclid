#pragma once

#include "axioms.h"
#include "cosmology/psyche.h"

namespace PROJECT_NAMESPACE {

class Canvas : public Name {
public:
    int width;
    int height;
    Signal<Pixel> &pixels;

    Canvas(int width, int height, Signal<Pixel> &pixels) :
            width{width},
            height{height},
            pixels{pixels} {}

    void paint(Point point, Pixel color) {
        int x = point.x;
        int y = point.y;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels.set_sample(x + (height - (y + 1)) * width, color);
        }
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
                auto new_position = Point::from_polar(step_distance, rotation);
                body.emplace_back(cur_tip.x + new_position.x, cur_tip.y + new_position.y);
            } else {
                if (not body.empty())
                    body.pop_back();
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
    Ray stalk;
    float energy_requirement;
    int depth;
    lst<Sunbeam> branching_beams;
    int age = 0;

    Sunbeam(Luon &luon, Point origin, float rotation, float energy_requirement, int depth) :
            luon{luon},
            stalk(luon, origin, rotation),
            energy_requirement{energy_requirement},
            depth{depth},
            branching_beams{} {
    }

    void draw(Canvas &canvas) {
        // color - luon energy
        auto color_mag = luon.energy * 10;
        auto red = 255 - trim(color_mag / 33, 0, 255);
        auto green = 255 - trim(color_mag / 3, 0, 255);
        auto blue = 255 - trim(color_mag * 33, 0, 255);

        stalk.paint_body(canvas, {red, green, blue});

        // replicate
        if (depth < MAX_DEPTH) {
            // trough detected
            if (luon.prior_delta <= 0 && luon.delta > 0) {
                auto origin = *(--stalk.body.end());
                auto rotation = stalk.rotation + cos(age / 4) * M_PI / 2;
                branching_beams.emplace_back(luon, origin, rotation, luon.energy, depth + 1);
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
    lst<Sunbeam> stalks;
    int age;

public:
    SunBeams(int width, int height, sp<Psyche> psyche)
            : width{width},
              height{height},
              psyche{psyche} {

    }

    SDL_Texture *observe() {
        age++;
        auto pixels = mkup<Signal<Pixel>>();
        pixels->populate(width * height, Pixel{255, 255, 255});
        int pitch = scast<int>(width * sizeof(Pixel));
        //////////////////////////////////////////////////////////////
        Canvas canvas{width, height, *pixels};

        for (auto &luon: *psyche->luons) {
            Point origin{scast<float>(rand() % width), scast<float>(rand() % height)};
            stalks.emplace_back(luon, origin, scast<float>(2 * M_PI * cos(age / 64)), 2, 0);
        }

        auto cur_stalk = stalks.begin();
        while (cur_stalk != stalks.end()) {
            if (not cur_stalk->live())
                cur_stalk = stalks.erase(cur_stalk);
            else
                cur_stalk++;
        }
        for (auto &stalk: stalks) {
            stalk.draw(canvas);
        }

        ////////////////////////////////////
        void *pixel_buffer;
        auto texture = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height
        );
        SDL_LockTexture(texture, nullptr, &pixel_buffer, &pitch);
        memcpy(pixel_buffer, pixels->get_data_pointer(), pixels->size() * sizeof(Pixel));
        SDL_UnlockTexture(texture);
        return texture;
    }
};

}
