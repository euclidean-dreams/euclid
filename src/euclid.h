#pragma once

#include "foundation.h"
#include "ingress/audio_input.h"
#include "ingress/stft.h"
#include "opus.h"
#include "cosmology/spectrogram.h"
#include "ingress/harmonic_transform.h"

namespace PROJECT_NAMESPACE {

// power of 2 >= 256
int FRAME_SIZE = 256;

int RENDER_TICK_INTERVAL = 4000;
int RENDER_WIDTH = 512;
int RENDER_HEIGHT = 900;

up<SDLAudioInput> audio_input;
up<FourierTransform> fourier_transform;
up<Opus> opus;
up<Spectrogram> cosmology;

uint64_t last_render_time = 0;

class Euclid : public Circlet {
private:
    void process_input() {
        auto audio_signal = audio_input->ring_buffer.get_next_signal();
        auto stft_signal = fourier_transform->stft(mv(audio_signal));
        if (stft_signal == nullptr) {
            return;
        }

        auto stft_magnitudes = mksp<Signal<float>>();
        for (auto &sample: *stft_signal) {
            auto magnitude = scast<float>(std::sqrt(std::pow(sample.real(), 2) + std::pow(sample.imag(), 2)));
            stft_magnitudes->push_back(magnitude);
        }

        cosmology->process(mv(stft_magnitudes));
    }

public:
    void activate() override {
        while (audio_input->ring_buffer.next_signal_is_ready()) {
            process_input();
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
    cosmology = mkup<Spectrogram>(RENDER_WIDTH, RENDER_HEIGHT);
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
        } else if (event.type == SDL_KEYDOWN) {

            auto symbol = event.key.keysym.sym;
            auto keymod = event.key.keysym.mod;
            auto shift_held_down = SDLK_LSHIFT & keymod;
            auto multiplier = 1;
            if (shift_held_down) {
                multiplier = 10;
            }
            if (symbol == SDLK_UP) {
                cosmology->shift_view(15 * multiplier);
            } else if (symbol == SDLK_DOWN) {
                cosmology->shift_view(-15 * multiplier);
            } else if (symbol == SDLK_RIGHTBRACKET) {
                cosmology->scale_color_gain(0.5 * multiplier);
            } else if (symbol == SDLK_LEFTBRACKET) {
                cosmology->scale_color_gain(-0.5 * multiplier);
            }
        }
    }
#endif
};

}
