#pragma once

#include "paradigm.h"
#include "acoustics/audio_input.h"
#include "acoustics/stft.h"
#include "optics/opus.h"
#include "cosmology/psyche.h"
#include "acoustics/equalizer.h"
#include "optics/sage.h"
#include "optics/atmosphere.h"
#include "cosmology.h"

namespace PROJECT_NAMESPACE {

// power of 2 >= 256
int FRAME_SIZE = 256;
int STFT_SIZE = (32 * FRAME_SIZE) / 2 + 1;
int LUON_COUNT = STFT_SIZE;

int RENDER_TICK_INTERVAL = 2000;
int render_width;
int render_height;

up<SDLAudioInput> audio_input;
up<Equalizer> equalizer;
up<FourierTransform> fourier_transform;
sp<Psyche> psyche;
up<Cosmology> current_cosmology;
up<Sage> sage;
up<Atmosphere> atmosphere;
up<Opus> opus;

uint64_t last_render_time = 0;
bool render_toggle = true;

class Euclid : public Circlet {
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
        psyche->perceive(stft_magnitudes);
    }

public:
    void activate() override {
        while (audio_input->ring_buffer.next_signal_is_ready()) {
            process_input();
        }
        if (get_current_time() - last_render_time > RENDER_TICK_INTERVAL) {
            last_render_time = get_current_time();
            SDL_Rect fullscreen{0, 0, render_width, render_height};
            Canvas blackout{render_width, render_height};
            blackout.paint_rect(fullscreen, {0, 0, 0, 255});
            opus->blit(blackout.finalize(), fullscreen);
            opus->blit(atmosphere->observe(), fullscreen);
            opus->blit(sage->observe(), fullscreen);

            opus->render();
        }
    }

    uint64_t get_tick_interval() override {
        return RENDER_TICK_INTERVAL;
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
#ifdef __EMSCRIPTEN__
    render_width = EM_ASM_INT(return screen.width);
    render_height = EM_ASM_INT(return screen.height);
#else
    render_width = 1470;
    render_height = 956;
#endif
    spdlog::info("render_width {}, render_height: {}", render_width, render_height);
    spdlog::info("(~) dimensions");

    spdlog::info("( ) renderer");
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
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

    spdlog::info("( ) acoustics");
    audio_input = mkup<SDLAudioInput>(FRAME_SIZE);
    equalizer = mkup<Equalizer>();
    fourier_transform = mkup<FourierTransform>();
    spdlog::info("(~) acoustics");

    spdlog::info("( ) cosmology");
    psyche = mksp<Psyche>(LUON_COUNT);
    spdlog::info("(~) cosmology");

    spdlog::info("( ) optics");
    vec<int> harmony_indices{};
    for (int i = 0; i < LUON_COUNT / 4; i++) {
        harmony_indices.push_back(i);
    }
    sage = mkup<Sage>(render_width, render_height, psyche->create_harmony(harmony_indices));

    harmony_indices.clear();
    for (int i = 0; i < LUON_COUNT / 16; i++) {
        harmony_indices.push_back(i);
    }
    atmosphere = mkup<Atmosphere>(render_width, render_height, psyche->create_harmony(harmony_indices));
    opus = mkup<Opus>();
    spdlog::info("(~) optics");

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(tick, 0, true);
    spdlog::info("(~) Initialized euclid");
#else
    auto thread = Circlet::begin(mv(euclid));
    SDL_Event event;
    bool running = true;
    spdlog::info("(~) Initialized euclid");
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
            if (symbol == SDLK_RIGHTBRACKET) {
                equalizer->scale_gain(0.1 * multiplier);
            } else if (symbol == SDLK_LEFTBRACKET) {
                equalizer->scale_gain(-0.1 * multiplier);
            } else if (symbol == SDLK_SPACE) {
                render_toggle = !render_toggle;
            }
        }
    }
#endif
};

}
