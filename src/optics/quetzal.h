#pragma once

#include "paradigm.h"

namespace euclid {

class SPIConnection : public Circlet {
private:
    std::vector<char> send_buffer;
    int spi_handle;
    sptr<Arbiter<Lattice>> observation_arbiter;
    uint8_t header_index = 0;

    void send(const unsigned char *data);

public:
    SPIConnection(sptr<Arbiter<Lattice>> observation_arbiter);

    void activate() override;

    uint64_t get_tick_interval() override;
};

class Quetzal : public Name {
private:
    sptr<Arbiter<Lattice>> observation_arbiter;
    uptr<std::thread> spi_thread;

public:
    Quetzal();

    void send(uptr<Lattice> lattice);
};

}
