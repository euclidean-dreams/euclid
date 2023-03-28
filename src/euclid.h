#pragma once

#include "foundation.h"
#include "ingress/audio_input.h"
#include "ingress/stft.h"
#include "opus.h"
#include "cosmology/cosmology.h"

namespace PROJECT_NAMESPACE {

int FRAME_SIZE = 512;
int RENDER_TICK_INTERVAL = 8000;
int RENDER_WIDTH = 512;
int RENDER_HEIGHT = 512;

up<SDLAudioInput> audio_input;
up<FourierTransform> fourier_transform;
up<Opus> opus;
up<Cosmology> cosmology;

uint64_t last_render_time = 0;

class Euclid : public Circlet {
public:
    void activate() override {
        while (audio_input->ring_buffer.next_signal_is_ready()) {
            auto audio_signal = audio_input->ring_buffer.get_next_signal();
            auto stft_signal = fourier_transform->stft(mv(audio_signal));
            if (stft_signal != nullptr) {
                cosmology->process(mv(stft_signal));
            }
        }
        if (get_current_time() - last_render_time > RENDER_TICK_INTERVAL) {
            last_render_time = get_current_time();
            auto pixels = cosmology->observe();
            opus->render(mv(pixels));
        }
    }

    uint64_t get_tick_interval() override {
        return RENDER_TICK_INTERVAL / 10;
    }
};

up<Euclid> euclid;

void tick() {
    euclid->activate();
}

void bootstrap() {
    spdlog::info("( ) Initializing euclid");
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    euclid = mkup<Euclid>();
    spdlog::info("( ) ingress");
    audio_input = mkup<SDLAudioInput>(FRAME_SIZE);
    fourier_transform = mkup<FourierTransform>();
    spdlog::info("(~) ingress");
    spdlog::info("( ) egress");
    opus = mkup<Opus>(RENDER_WIDTH, RENDER_HEIGHT);
    spdlog::info("(~) egress");
    spdlog::info("( ) cosmology");
    cosmology = mkup<Cosmology>(RENDER_WIDTH, RENDER_HEIGHT);
    spdlog::info("(~) cosmology");
    spdlog::info("(~) Initialized euclid");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, true);
#else
    auto thread = Circlet::begin(mv(euclid));

    SDL_Event event;
    bool running = true;
    while (running) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
#endif
};

}
