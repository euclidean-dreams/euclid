#pragma once

#include "foundation.h"
#include "ingress/audio_input.h"
#include "ingress/stft.h"
#include "egress/opus.h"
#include "cosmology/psyche.h"
#include "egress/luon_spiral.h"
#include "egress/spectrogram.h"
#include "ingress/equalizer.h"

namespace PROJECT_NAMESPACE {

// power of 2 >= 256
int FRAME_SIZE = 256;
int LUON_COUNT = (32 * FRAME_SIZE) / 2 + 1;

int RENDER_TICK_INTERVAL = 2000;
int render_width;
int render_height;

up<SDLAudioInput> audio_input;
up<Equalizer> equalizer;
up<FourierTransform> fourier_transform;
sp<Psyche> psyche;
up<Spectrogram> spectrogram;
up<LuonSpiral> luon_spiral;
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
        spectrogram->process(stft_magnitudes);
        psyche->perceive(stft_magnitudes);
    }

public:
    void activate() override {
        while (audio_input->ring_buffer.next_signal_is_ready()) {
            process_input();
        }
        if (get_current_time() - last_render_time > RENDER_TICK_INTERVAL) {
            last_render_time = get_current_time();
            auto spectrogram_texture = spectrogram->observe();
            SDL_Rect left_half;
            left_half.x = 0;
            left_half.y = 0;
            left_half.w = render_width / 2;
            left_half.h = render_height;
            opus->blit(spectrogram_texture, left_half);


            auto luon_texture = luon_spiral->observe();
            SDL_Rect right_half;
            right_half.x = render_width / 2;
            right_half.y = 0;
            right_half.w = render_width / 2;
            right_half.h = render_height;
            opus->blit(luon_texture, right_half);

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
    render_width = 1400;
    render_height = 930;
#endif
    spdlog::info("render_width {}, render_height: {}", render_width, render_height);
    spdlog::info("(~) dimensions");

    spdlog::info("( ) renderer");
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    window = SDL_CreateWindow(
            "euclid",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            render_width,
            render_height,
            SDL_WINDOW_BORDERLESS
    );
    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
    renderer = SDL_CreateRenderer(window, -1, rendererFlags);
    spdlog::info("(~) renderer");

    spdlog::info("( ) ingress");
    audio_input = mkup<SDLAudioInput>(FRAME_SIZE);
    equalizer = mkup<Equalizer>();
    fourier_transform = mkup<FourierTransform>();
    spdlog::info("(~) ingress");

    spdlog::info("( ) cosmology");
    psyche = mksp<Psyche>(LUON_COUNT);
    spdlog::info("(~) cosmology");

    spdlog::info("( ) egress");
    auto widths = render_width / 2;
    auto heights = render_height;
    spectrogram = mkup<Spectrogram>(widths, heights);
    luon_spiral = mkup<LuonSpiral>(widths, heights, psyche);
    opus = mkup<Opus>();
    spdlog::info("(~) egress");

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
            if (symbol == SDLK_UP) {
                spectrogram->shift_view(15 * multiplier);
            } else if (symbol == SDLK_DOWN) {
                spectrogram->shift_view(-15 * multiplier);
            } else if (symbol == SDLK_RIGHTBRACKET) {
                equalizer->scale_gain(0.5 * multiplier);
            } else if (symbol == SDLK_LEFTBRACKET) {
                equalizer->scale_gain(-0.5 * multiplier);
            } else if (symbol == SDLK_SPACE) {
                render_toggle = !render_toggle;
            }
        }
    }
#endif
};

}
