#pragma once

#include "paradigm.h"

namespace euclid {
class QuetzalPacket : public Name {
public:
    vect<unsigned char> samples;

    QuetzalPacket(Lattice &lattice) : samples{} {
        samples.reserve(lattice.size() * 3);
        for (int y = 0; y < lattice.height; y++) {
            for (int x = 0; x < lattice.width; x++) {
                auto color = lattice.get_pith(x, y).color;
                samples.push_back(color.red);
                samples.push_back(color.green);
                samples.push_back(color.blue);
            }
        }
    }

    QuetzalPacket(Signal<int> &signal) : samples{} {
        samples.reserve(signal.size());
        for (auto &sample: signal) {
            samples.push_back(sample);
        }
    }
};

class SPIConnection : public Circlet {
private:
    std::vector<char> send_buffer;
    int spi_handle;
    sptr<Arbiter<QuetzalPacket>> observation_arbiter;
    uint8_t header_index = 0;

    void send(const unsigned char *data);

public:
    SPIConnection(sptr<Arbiter<QuetzalPacket>> observation_arbiter);

    void activate() override;

    uint64_t get_tick_interval() override;
};

class Quetzal : public Name {
private:
    sptr<Arbiter<QuetzalPacket>> observation_arbiter;
    uptr<std::thread> spi_thread;

public:
    Quetzal();

    void send(uptr<QuetzalPacket> packet);
};
}
