#pragma once

#include "paradigm.h"
#include "acoustics/audio_input.h"
#include "acoustics/stft.h"
#include "acoustics/equalizer.h"
#include "cosmology.h"

#ifdef OPUS
#include "optics/opus.h"
#include "input/fascia.h"
#endif
#ifdef WAVELET
#include "optics/wavelet.h"
#endif

namespace euclid {

// power of 2 >= 256
int FRAME_SIZE = 256;
int STFT_SIZE = (32 * FRAME_SIZE) / 2 + 1;

int RENDER_TICK_INTERVAL = 9000;
int render_width;
int render_height;
bool running = true;

up<SDLAudioInput> audio_input;
up<Equalizer> equalizer;
up<FourierTransform> fourier_transform;
up<Cosmology> cosmos;
#ifdef OPUS
up<Opus> opus;
up<Fascia> fascia;
SDL_Window *window;
SDL_Renderer *renderer;
#endif
#ifdef WAVELET
up<Wavelet> wavelet;
#endif

uint64_t last_render_time = 0;

class Euclid : public Name {
private:
    void process_input() {
        auto audio = audio_input->ring_buffer.get_next_signal();
        auto equalized = equalizer->equalize(mv(audio));
        auto stft = fourier_transform->stft(mv(equalized));
        if (stft == nullptr) {
            return;
        }

        auto stft_magnitudes = mksp<Signal<float>>();
        for (auto &sample: *stft) {
            auto magnitude = scast<float>(std::sqrt(std::pow(sample.real(), 2) + std::pow(sample.imag(), 2)));
            stft_magnitudes->push_back(magnitude);
        }
        cosmos->experience(stft_magnitudes);
    }

public:
    void activate() {
        while (audio_input->ring_buffer.next_signal_is_ready()) {
            process_input();
        }
        if (get_current_time() - last_render_time > RENDER_TICK_INTERVAL) {
            last_render_time = get_current_time();
            auto lattice = cosmos->observe();
#ifdef OPUS
            if (lattice != nullptr) {
                Canvas canvas{*lattice};
                opus->blit(canvas.finalize(), canvas.area);
            }
            auto canvas = fascia->observe();
            opus->blit(canvas->finalize(), canvas->area);
            opus->render();
            fascia->handle_events();
#endif
#ifdef WAVELET
            wavelet->send(mv(lattice));
#endif
        }
    }
};

up<Euclid> euclid;

void tick() {
    euclid->activate();
}

void bootstrap() {
    spdlog::info("( ) Initializing euclid");
    euclid = mkup<Euclid>();

    spdlog::info("( ) dimensions");
#ifdef WASM
    render_width = EM_ASM_INT(return screen.width);
    render_height = EM_ASM_INT(return screen.height);
#endif
#ifdef MAC
    render_width = 1470;
    render_height = 956;
#endif
#ifdef SJOFN
    render_width = 64;
    render_height = 64;
#endif
    spdlog::info("render_width {}, render_height: {}", render_width, render_height);
    spdlog::info("(~) dimensions");

    spdlog::info("( ) acoustics");
    SDL_Init(SDL_INIT_AUDIO);
    audio_input = mkup<SDLAudioInput>(FRAME_SIZE);
    equalizer = mkup<Equalizer>();
    fourier_transform = mkup<FourierTransform>();
    spdlog::info("(~) acoustics");

    spdlog::info("( ) cosmology");
    cosmos = mkup<Cosmology>(render_width, render_height, STFT_SIZE);
    spdlog::info("(~) cosmology");

    spdlog::info("( ) optics");
#ifdef OPUS
    spdlog::info("( ) renderer");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    Uint32 window_flags = SDL_WINDOW_BORDERLESS;
    window = SDL_CreateWindow(
            "euclid",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            render_width,
            render_height,
            window_flags
    );
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    renderer = SDL_CreateRenderer(window, -1, rendererFlags);
    spdlog::info("(~) renderer");
    opus = mkup<Opus>();
    fascia = mkup<Fascia>(*equalizer);
#endif
#ifdef WAVELET
    wavelet = mkup<Wavelet>();
#endif
    spdlog::info("(~) optics");
    spdlog::info("(~) Initialized euclid");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, true);
#else
    while (running) {
        tick();
    }
#endif
};

}
