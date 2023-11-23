#pragma once

#include "paradigm.h"

namespace euclid {

class SPIConnection : public Circlet {
private:
    std::vector<char> send_buffer;
    int spi_handle;
    sp<Arbiter<Lattice>> observation_arbiter;

    void send(const unsigned char *data);

public:
    SPIConnection(sp<Arbiter<Lattice>> observation_arbiter);

    void activate() override;

    uint64_t get_tick_interval() override;
};

class Wavelet : public Name {
private:
    sp<Arbiter<Lattice>> observation_arbiter;
    up<std::thread> spi_thread;

public:
    Wavelet();

    void send(up<Lattice> lattice);

};

}
