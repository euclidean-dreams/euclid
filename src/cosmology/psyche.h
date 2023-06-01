#pragma once

#include "axioms.h"

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
private:
    lst<Luon> luons;

public:
    Harmony(vec<int> &signal_indices) : luons{} {
        for (auto index: signal_indices) {
            luons.emplace_back(index);
        }
    }

    float perceive(sp<Signal<float>> &signal) {
        float energy = 0;
        for (auto &luon: luons) {
            luon.excite(signal);
            energy += luon.energy;
        }
        return energy;
    }
};


class Psyche : public Name {
public:
    sp<lst<Luon>> luons;


    Psyche(int luon_count) {
        luons = mksp<lst<Luon>>();
        for (int fundamental = 0; fundamental < luon_count; fundamental++) {
            luons->emplace_back(fundamental);
        }
    }

    void perceive(sp<Signal<float>> &signal) {
        auto result = mksp<Signal<float>>();
        for (auto &luon: *luons) {
            luon.excite(signal);
        }
    }
};

}
