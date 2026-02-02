#pragma once

#include "paradigm.h"


namespace euclid {
class Fixture : public Name {
protected:
    Coordinate bottom_left_watch_point; // inclusive
    Coordinate upper_right_watch_point; // exclusive
    int width;
    int height;
    int point_count;
    int address;

public:
    Fixture(Coordinate bottom_left_watch_point, Coordinate upper_right_watch_point, int address) :
        bottom_left_watch_point{bottom_left_watch_point},
        upper_right_watch_point{upper_right_watch_point},
        width{upper_right_watch_point.x - bottom_left_watch_point.x},
        height{upper_right_watch_point.y - bottom_left_watch_point.y},
        point_count{width * height},
        address{address} {
    }

    virtual void interpret(Lattice &lattice, Signal<int> &dmx_frame) {
    }
};

class Rig : public Name {
private:
    vect<uptr<Fixture>> fixtures;

public:
    Rig() : fixtures{} {
    }

    void add_fixture(uptr<Fixture> fixture) {
        fixtures.push_back(mv(fixture));
    }

    uptr<Signal<int>> interpret(Lattice &lattice) {
        auto dmx_frame = mkuptr<Signal<int>>();
        while (dmx_frame->size() < 512) {
            dmx_frame->push_back(0);
        }
        for (auto &fixture: fixtures) {
            fixture->interpret(lattice, *dmx_frame);
        }
        return dmx_frame;
    }
};
}
