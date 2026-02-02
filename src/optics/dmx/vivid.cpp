#include "vivid.h"


namespace euclid {
void Vivid::interpret(Lattice &lattice, Signal<int> &dmx_frame) {
    auto red_sum = 0;
    auto green_sum = 0;
    auto blue_sum = 0;
    for (int x = bottom_left_watch_point.x; x < upper_right_watch_point.x; x++) {
        for (int y = bottom_left_watch_point.y; y < upper_right_watch_point.y; y++) {
            auto pith = lattice.get_pith(x, y);
            red_sum += pith.color.red;
            green_sum += pith.color.green;
            blue_sum += pith.color.blue;
        }
    }
    auto red = red_sum / point_count;
    auto green = green_sum / point_count;
    auto blue = blue_sum / point_count;

    auto offset = address - 1;

    dmx_frame.set_sample(offset, red);
    dmx_frame.set_sample(offset + 1, red / 2);
    dmx_frame.set_sample(offset + 2, red / 3);
    // dmx_frame.set_sample(offset + 3, green);
    // dmx_frame.set_sample(offset + 4, green / 2);
    dmx_frame.set_sample(offset + 5, blue);
    dmx_frame.set_sample(offset + 6, blue / 2);
    dmx_frame.set_sample(offset + 7, 255);
}
}
