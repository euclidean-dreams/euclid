#pragma once

#include <thread>
#include "paradigm.h"
#include "./time.h"

namespace PROJECT_NAMESPACE {

class Entity {
public:
    Entity(const Entity &) = delete;

    Entity &operator=(const Entity &) = delete;

protected:
    Entity() = default;
};

class Circlet : public Entity {
public:
    virtual ~Circlet() = default;

    virtual void activate() = 0;

    virtual bool finished() { return false; };

    virtual uint64_t get_tick_interval() = 0;

    static up<std::thread> begin(up<Circlet> circlet) {
        auto thread = mkup<std::thread>(circle, mv(circlet));
        return thread;
    }

    static void circle(up<Circlet> circlet) {
        while (!circlet->finished()) {
            auto cycleStartTime = get_current_time();
            circlet->activate();
            auto totalCycleTime = get_elapsed_time(cycleStartTime);
            if (circlet->get_tick_interval() > totalCycleTime) {
                sleep(circlet->get_tick_interval() - totalCycleTime);
            }
        }
    }
};


}
