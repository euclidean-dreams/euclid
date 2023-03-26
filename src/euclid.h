#pragma once

#include "foundation.h"
#include "ingress/audio_input.h"
#include "ingress/stft.h"
#include "egress/opus.h"

namespace PROJECT_NAMESPACE {

int FRAME_SIZE = 512;
int RENDER_TICK_INTERVAL = 10000;

up<SDLAudioInput> audio_input;
up<FourierTransform> fourier_transform;
up<Opus> opus;

uint64_t last_render_time;

void tick() {
    spdlog::debug("~tick~");
    while (audio_input->ring_buffer.next_signal_is_ready()) {
        auto audio_signal = audio_input->ring_buffer.get_next_signal();
        spdlog::debug("sig: {} {}", audio_signal->get_sample(1), audio_signal->get_sample(2));

        auto stft_signal = fourier_transform->stft(mv(audio_signal));
        spdlog::debug("stft: {} {}", stft_signal->get_sample(1).real(), stft_signal->get_sample(2).real());
    }
    if (get_current_time() - last_render_time > RENDER_TICK_INTERVAL) {
        last_render_time = get_current_time();
        auto texture = opus->create_texture();
        opus->render(texture);
        SDL_DestroyTexture(texture);
    }
}

void bootstrap() {
    spdlog::info("( ) Initializing euclid");
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    spdlog::info("( ) ingress");
    audio_input = mkup<SDLAudioInput>(FRAME_SIZE);
    fourier_transform = mkup<FourierTransform>();
    spdlog::info("(~) ingress");
    spdlog::info("( ) egress");
    opus = mkup<Opus>();
    spdlog::info("(~) egress");
    spdlog::info("(~) Initialized euclid");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, true);
#else
    SDL_Event event;
    bool running = true;
    while (running) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT) {
            running = false;
        }
        tick();
    }
#endif
};

}
