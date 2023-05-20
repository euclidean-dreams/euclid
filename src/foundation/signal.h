#pragma once

#include "paradigm.h"
#include "interfaces.h"

namespace PROJECT_NAMESPACE {

template<class T>
class Signal : public Entity {
private:
    vec<T> samples{};

public:
    void push_back(T sample) {
        samples.push_back(sample);
    }

    T get_sample(int index) {
        return samples[index];
    }

    void set_sample(int index, T sample) {
        samples[index] = sample;
    }

    void populate(int size, T value) {
        samples.clear();
        samples.resize(size, value);
    }

    typename vec<T>::iterator begin() {
        return samples.begin();
    }

    typename vec<T>::iterator end() {
        return samples.end();
    }

    T *get_data_pointer() {
        return samples.data();
    }

    size_t size() {
        return samples.size();
    }
};


}
