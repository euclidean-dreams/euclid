#pragma once

#include <chrono>
#include <thread>
#include "paradigm.h"

namespace PROJECT_NAMESPACE {

uint64_t get_current_time() {
    auto rawTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now());
    return rawTime.time_since_epoch().count();
}

uint64_t get_elapsed_time(uint64_t initial_time) {
    return get_current_time() - initial_time;
}

void sleep(uint64_t microseconds) {
    std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
}

class Timer {
private:
    uint64_t start_time{get_current_time()};

public:
    uint64_t get_time() const {
        return get_elapsed_time(start_time);
    }

    void log_time() const {
        spdlog::info(get_time());
    }
};

}
