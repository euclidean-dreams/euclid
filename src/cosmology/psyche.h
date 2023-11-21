#pragma once

#include "paradigm.h"

namespace PROJECT_NAMESPACE {


class Luon : public Name {
public:
    float energy = 0;
    float prior_energy = 0;
    float delta = 0;
    float prior_delta = 0;
    int fundamental;

    Luon(int fundamental) :
            fundamental{fundamental} {
    }

    void excite(sp<Signal<float>> &signal) {
        prior_energy = energy;
        energy = signal->get_sample(fundamental);

        prior_delta = delta;
        delta = energy - prior_energy;
    }
};


class Harmony : public Name {
public:
    up<vec<sp<Luon>>> luons;

    Harmony(up<vec<sp<Luon>>> luons) : luons{mv(luons)} {

    }
};


class Psyche : public Name {
private:
    vec<sp<Luon>> luons;

public:
    Psyche(int luon_count) : luons{} {
        for (int fundamental = 0; fundamental < luon_count; fundamental++) {
            luons.push_back(mksp<Luon>(fundamental));
        }
    }

    void perceive(sp<Signal<float>> &signal) {
        for (auto &luon: luons) {
            luon->excite(signal);
        }
    }

    up<Harmony> create_harmony(vec<int> &signal_indices) {
        auto luons_in_harmony = mkup<vec<sp<Luon>>>();
        for (auto index: signal_indices) {
            luons_in_harmony->push_back(luons[index]);
        }
        return mkup<Harmony>(mv(luons_in_harmony));
    }
};

}
