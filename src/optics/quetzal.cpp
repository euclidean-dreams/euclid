#include "quetzal.h"
#include <pigpio.h>

#define HEADER_SIZE 16
#define LED_COUNT (render_width * render_height)
#ifdef QUETZAL_DMX
#define SPI_PACKET_SIZE (HEADER_SIZE + 512)
#define FRAME_RATE 33333
#endif
#ifdef QUETZAL_KEYHOLE
#define SPI_PACKET_SIZE (HEADER_SIZE + LED_COUNT * 3)
#define FRAME_RATE 16000
#endif
#define BAUDRATE (8 * 1000 * 1000)

namespace euclid {
SPIConnection::SPIConnection(sptr<Arbiter<QuetzalPacket>> observation_arbiter)
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
    salutation.push_back(header_index);
    salutation.push_back(0);

    auto spi_packet_size_stamp = std::bit_cast<std::array<unsigned char, sizeof(int)>>(
        static_cast<uint32_t>(SPI_PACKET_SIZE));
    salutation.push_back(spi_packet_size_stamp[0]);
    salutation.push_back(spi_packet_size_stamp[1]);
    salutation.push_back(spi_packet_size_stamp[2]);
    salutation.push_back(spi_packet_size_stamp[3]);
    salutation.push_back(0);
    salutation.push_back(0);
    salutation.push_back(0);
    salutation.push_back(0);

    header_index++;
    header_index %= 256;

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
        auto quetzal_packet = observation_arbiter->take();
        vect<unsigned char> spi_packet;
        spi_packet.reserve(SPI_PACKET_SIZE);

        // header
        spi_packet.push_back(1);
        spi_packet.push_back(2);
        spi_packet.push_back(4);
        spi_packet.push_back(8);
        spi_packet.push_back(7);
        spi_packet.push_back(5);
        spi_packet.push_back(header_index);
        spi_packet.push_back(0);

        auto spi_packet_size_stamp = std::bit_cast<std::array<unsigned char, sizeof(int)>>(
            static_cast<uint32_t>(SPI_PACKET_SIZE));
        spi_packet.push_back(spi_packet_size_stamp[0]);
        spi_packet.push_back(spi_packet_size_stamp[1]);
        spi_packet.push_back(spi_packet_size_stamp[2]);
        spi_packet.push_back(spi_packet_size_stamp[3]);
        spi_packet.push_back(0);
        spi_packet.push_back(0);
        spi_packet.push_back(0);
        spi_packet.push_back(0);

        header_index++;
        header_index %= 256;

        for (auto &sample: quetzal_packet->samples) {
            spi_packet.push_back(sample);
        }
        send(spi_packet.data());
    }
}

uint64_t SPIConnection::get_tick_interval() {
    return FRAME_RATE;
}

Quetzal::Quetzal() :
    observation_arbiter{} {
    observation_arbiter = mksptr<Arbiter<QuetzalPacket>>();
    auto spi_connection = mkuptr<SPIConnection>(observation_arbiter);
    spi_thread = Circlet::begin(mv(spi_connection));
}

void Quetzal::send(std::unique_ptr<QuetzalPacket> packet) {
    observation_arbiter->give(mv(packet));
}
}
