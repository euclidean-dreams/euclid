#pragma once

#include "foundation.h"
#include "shaping/audio_input.h"
#include "shaping/stft.h"

namespace PROJECT_NAMESPACE {

int FRAME_SIZE = 512;

up<SDLAudioInput> audio_input;
up<FourierTransform> fourierTransform;

void tick() {
    spdlog::info("~tick~");
    while (audio_input->ring_buffer.next_signal_is_ready()) {
        auto audio_signal = audio_input->ring_buffer.get_next_signal();
        spdlog::debug("sig: {} {}", audio_signal->get_sample(1), audio_signal->get_sample(2));

        auto stft_signal = fourierTransform->stft(mv(audio_signal));
        spdlog::info("stft: {} {}", stft_signal->get_sample(1).real(), stft_signal->get_sample(2).real());
    }
}

void bootstrap() {
    spdlog::info("( ) Initializing euclid");
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    spdlog::info("( ) audio");
    audio_input = mkup<SDLAudioInput>(FRAME_SIZE);
    fourierTransform = mkup<FourierTransform>();
    spdlog::info("(~) audio");
    spdlog::info("(~) Initialized euclid");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, true);
#else
    while (true) {
        audio_input->ring_buffer.wait_until_next_signal();
        tick();
    }
#endif
};

}
