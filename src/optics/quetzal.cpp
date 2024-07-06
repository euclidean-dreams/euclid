#include "quetzal.h"
#include <pigpio.h>

#define HEADER_SIZE 8
#define LED_COUNT (render_width * render_height)
#define SPI_PACKET_SIZE (HEADER_SIZE + LED_COUNT * 3)
#define BAUDRATE (8 * 1000 * 1000)
#define FRAME_RATE 16000

namespace euclid {

SPIConnection::SPIConnection(sptr<Arbiter<Lattice>> observation_arbiter)
        : send_buffer{},
          observation_arbiter{mv(observation_arbiter)} {
    spdlog::info("( ) spi connection");
    spdlog::info("initializing pigpio");
    auto init_result = gpioInitialise();
    if (init_result < 0) {
        spdlog::error("failed to initialize pigpio - failed with error code: {}", init_result);
    }

    spdlog::info("opening SPI connection");
    send_buffer.resize(SPI_PACKET_SIZE, 0);
    spi_handle = spiOpen(0, BAUDRATE, 0);
    if (spi_handle < 0) {
        spdlog::error("failed to open spi connection, error code: {}", spi_handle);
    }

    spdlog::info("offering SPI salutation!");
    vect<unsigned char> salutation;
    salutation.reserve(SPI_PACKET_SIZE);
    salutation.push_back(1);
    salutation.push_back(2);
    salutation.push_back(4);
    salutation.push_back(8);
    salutation.push_back(7);
    salutation.push_back(5);
    salutation.push_back(0);
    salutation.push_back(0);

    for (int i = 0; i < 2; i++) {
        salutation.push_back(77);
        salutation.push_back(33);
        salutation.push_back(0);
    }
    for (int i = 0; i < 4; i++) {
        salutation.push_back(0);
        salutation.push_back(0);
        salutation.push_back(0);
    }
    for (int i = 0; i < 2; i++) {
        salutation.push_back(33);
        salutation.push_back(77);
        salutation.push_back(0);
    }
    for (int i = 0; i < LED_COUNT - 8; i++) {
        salutation.push_back(0);
        salutation.push_back(0);
        salutation.push_back(0);
    }
    send(salutation.data());
    spdlog::info("(*) spi connection");
}

void SPIConnection::send(const unsigned char *data) {
    std::memcpy(send_buffer.data(), data, SPI_PACKET_SIZE);
    auto spiResult = spiWrite(spi_handle, send_buffer.data(), SPI_PACKET_SIZE);
    if (spiResult != SPI_PACKET_SIZE) {
        spdlog::info("spi write failed, error code: {}", spiResult);
    }
}

void SPIConnection::activate() {
    if (observation_arbiter->ready()) {
        auto lattice = observation_arbiter->take();
        vect<unsigned char> new_data;
        new_data.reserve(SPI_PACKET_SIZE);

        // header
        new_data.push_back(1);
        new_data.push_back(2);
        new_data.push_back(4);
        new_data.push_back(8);
        new_data.push_back(7);
        new_data.push_back(5);
        new_data.push_back(0);
        new_data.push_back(0);

        for (int y = lattice->height - 1; y >= 0; y--) {
            for (int x = 0; x < lattice->width; x++) {
                auto color = lattice->get_color(x, y);
                new_data.push_back(color.red);
                new_data.push_back(color.green);
                new_data.push_back(color.blue);
            }
        }
        send(new_data.data());
    }
}

uint64_t SPIConnection::get_tick_interval() {
    return FRAME_RATE;
}

Quetzal::Quetzal() :
        observation_arbiter{} {
    observation_arbiter = mksptr<Arbiter<Lattice>>();
    auto spi_connection = mkuptr<SPIConnection>(observation_arbiter);
    spi_thread = Circlet::begin(mv(spi_connection));
}

void Quetzal::send(std::unique_ptr<Lattice> lattice) {
    observation_arbiter->give(mv(lattice));
}

}
