#pragma once

#include "axioms.h"
#include "acoustics/audio_input.h"
#include "acoustics/stft.h"
#include "optics/opus.h"
#include "cosmology/psyche.h"
#include "acoustics/equalizer.h"
#include "optics/brushwork.h"

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
up<Brushwork> brushwork;
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
            auto luon_texture = brushwork->observe();
            SDL_Rect fullscreen{
                    0,
                    0,
                    render_width,
                    render_height
            };
            fullscreen.x = 0;
            fullscreen.y = 0;
            fullscreen.w = render_width;
            fullscreen.h = render_height;
            opus->blit(luon_texture, fullscreen);

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
    brushwork = mkup<Brushwork>(render_width, render_height, psyche);
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
