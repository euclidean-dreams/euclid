#pragma once

#include "paradigm.h"
#include "rig.h"


namespace euclid {
class Vivid : public Fixture {
public:
    Vivid(Coordinate bottom_left_watch_point, Coordinate upper_right_watch_point, int address) :
        Fixture{
            bottom_left_watch_point, upper_right_watch_point, address
        } {
    }

    void interpret(Lattice &lattice, Signal<int> &dmx_frame);
};
}
