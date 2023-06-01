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

class Foliage : public Name {
public:
    Luon &luon;
    Point origin;
    float rotation;
    float energy_requirement;
    lst<Point> body;
    int age = 0;

    Foliage(Luon &luon, Point origin, float rotation, float energy_requirement) :
            luon{luon},
            origin{origin},
            rotation{rotation},
            energy_requirement{energy_requirement},
            body{} {

    }

    void paint_body(Canvas &canvas, Pixel color) {
        for (auto &point: body) {
            canvas.paint(point, color);
        }
    }

    bool live() {
        age += 1;
        // return true to continue being alive
        if (luon.energy >= energy_requirement)
            return true;
        return false;
    }
};

class Leaf : public Foliage {
public:
    Leaf(Luon &luon, Point origin, float rotation, float energy_requirement) :
            Foliage(luon, origin, rotation, energy_requirement) {
    }

    void show(Canvas &canvas) {
        // color - luon energy
        auto color_mag = luon.energy;
        auto red = 255 - trim(color_mag / 33, 0, 255);
        auto green = 255 - trim(color_mag / 3, 0, 255);
        auto blue = 255 - trim(color_mag * 33, 0, 255);

        body.clear();
        int scale = luon.energy / 23;
        for (int x = origin.x; x < origin.x + scale; x++) {
            for (int y = origin.y; y < origin.y + scale; y++) {
                body.emplace_back(x, y);
            }
        }

        paint_body(canvas, {red, green, blue});
    }
};

class Branch : public Foliage {
public:
    lst<Leaf> leaves;

    Branch(Luon &luon, Point origin, float rotation, float energy_requirement) :
            Foliage(luon, origin, rotation, energy_requirement),
            leaves{} {
        body.push_back(origin);
    }

    void show(Canvas &canvas) {
        int displacement = abs(luon.delta * 2);
        while (displacement > 0) {
            auto step_distance = 0.5;
            displacement -= step_distance;
            if (luon.delta > 0) {
                auto cur_tip = *(--body.end());
                auto new_position = Point::from_polar(step_distance, rotation);
                body.emplace_back(cur_tip.x + new_position.x, cur_tip.y + new_position.y);
            } else {
                if (not body.empty())
                    body.pop_back();
            }
        }

        // trough detected
        if (luon.prior_delta <= 0 && luon.delta > 0) {
            leaves.emplace_back(luon, *(--body.end()), 0, luon.energy);
        }

        // color - luon energy
        auto color_mag = luon.energy * 5;
        auto red = 255 - trim(color_mag / 33, 0, 255);
        auto green = 255 - trim(color_mag / 3, 0, 255);
        auto blue = 255 - trim(color_mag * 33, 0, 255);

        paint_body(canvas, {red, green, blue});

        auto cur_leaf = leaves.begin();
        while (cur_leaf != leaves.end()) {
            if (not cur_leaf->live())
                cur_leaf = leaves.erase(cur_leaf);
            else
                cur_leaf++;
        }
        for (auto &leaf: leaves) {
            leaf.show(canvas);
        }
    }
};

class Stalk : public Foliage {
public:
    lst<Branch> branches;

    Stalk(Luon &luon, Point origin, float rotation, float energy_requirement) :
            Foliage(luon, origin, rotation, energy_requirement),
            branches{} {
        body.push_back(origin);
    }

    void draw(Canvas &canvas) {
        int displacement = abs(luon.delta * 2);
        while (displacement > 0) {
            auto step_distance = 0.5;
            displacement -= step_distance;
            if (luon.delta > 0) {
                auto cur_tip = *(--body.end());
                auto new_position = Point::from_polar(step_distance, rotation);
                body.emplace_back(cur_tip.x + new_position.x, cur_tip.y + new_position.y);
            } else {
                if (not body.empty())
                    body.pop_back();
            }
        }

        // color - luon energy
        auto color_mag = luon.energy * 10;
        auto red = 255 - trim(color_mag / 33, 0, 255);
        auto green = 255 - trim(color_mag / 3, 0, 255);
        auto blue = 255 - trim(color_mag * 33, 0, 255);

        paint_body(canvas, {red, green, blue});

        // trough detected
        if (luon.prior_delta <= 0 && luon.delta > 0) {
            branches.emplace_back(luon, *(--body.end()), cos(age / 16) * M_PI / 3, luon.energy);
        }

        auto cur_branch = branches.begin();
        while (cur_branch != branches.end()) {
            if (not cur_branch->live())
                cur_branch = branches.erase(cur_branch);
            else
                cur_branch++;
        }
        for (auto &branch: branches) {
            branch.show(canvas);
        }
    }
};

class Verdant : public Name {
private:
    int width;
    int height;
    sp<Psyche> psyche;
    lst<Stalk> stalks;
    int age;

public:
    Verdant(int width, int height, sp<Psyche> psyche)
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
            stalks.emplace_back(luon, origin, scast<float>(2 * M_PI * cos(age / 64)), 3);
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
