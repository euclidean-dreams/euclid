#pragma once

#include "paradigm.h"
#include "perception/equalizer.h"
#include "cosmology.h"

#ifdef KFR_FFT

#include "acoustics/kfr_fft.h"

#endif
#ifdef KISS_FFT
#include "acoustics/kiss_fft.h"
#endif

// acoustics
#ifdef SDL_AUDIO

#include "acoustics/sdl_input.h"

#endif
#ifdef PORT_AUDIO

#include "acoustics/portaudio_input.h"

#endif

// optics
#ifdef OPUS

#include "optics/opus.h"
#include "interaction/fascia.h"
#include "impressions/juice.h"

#endif
#ifdef QUETZAL

#include "optics/quetzal.h"

#endif

namespace euclid {

int RENDER_TICK_INTERVAL = 9000;
int render_width;
int render_height;
bool running = true;

uptr<Equalizer> equalizer;
uptr<FourierTransform> fourier_transform;
vect<uptr<Cosmology>> cosmos;
#ifdef SDL_AUDIO
uptr<SDLAudioInput> audio_input;
#endif
#ifdef PORT_AUDIO
uptr<PortaudioInput> audio_input;
#endif

#ifdef MAC
uptr<Fascia> fascia;
#endif

#ifdef OPUS
uptr<Opus> opus;
SDL_Window *window;
SDL_Renderer *renderer;
#endif
#ifdef QUETZAL
uptr<Quetzal> quetzal;
#endif

uint64_t last_render_time = 0;

class Euclid : public Name {
private:
    bool already_logged_first_audio_signal = false;

    void process_input() {
        auto audio = audio_input->ring_buffer.get_next_signal();
        if (not already_logged_first_audio_signal) {
            spdlog::info("received first audio signal:");
            spdlog::info("    {}, {}, {}, {}, {}, {}, {}",
                         audio->get_sample(0), audio->get_sample(1), audio->get_sample(2), audio->get_sample(3),
                         audio->get_sample(4), audio->get_sample(5), audio->get_sample(6));
            already_logged_first_audio_signal = true;
        }
        auto equalized = equalizer->equalize(mv(audio));
        auto stft = fourier_transform->stft(mv(equalized));
        if (stft == nullptr) {
            return;
        }

        auto stft_magnitudes = mksptr<Signal<float>>();
        for (auto &sample: *stft) {
            auto magnitude = scast<float>(std::sqrt(std::pow(sample.real(), 2) + std::pow(sample.imag(), 2)));
            stft_magnitudes->push_back(magnitude);
        }
        for (auto &cosmology: cosmos) {
            cosmology->experience(stft_magnitudes);
        }
    }

public:
    void activate() {
        while (audio_input->ring_buffer.next_signal_is_ready()) {
            process_input();
        }
        if (get_current_time() - last_render_time > RENDER_TICK_INTERVAL) {
            last_render_time = get_current_time();

            for (auto &cosmology: cosmos) {
                auto lattice = cosmology->observe();
#ifdef OPUS
                if (lattice != nullptr) {
                    opus->fill(lattice->null_pith.color);
                    if (lattice->tessellate) {
                        Tesselation tesselation{*lattice};
                        opus->blit(tesselation.finalize(), tesselation.area);
                    } else {
                        Canvas canvas{*lattice};
                        opus->blit(canvas.finalize(), canvas.area);
                    }
                }
            }
#ifdef MAC
            auto canvas = fascia->observe();
            opus->blit(canvas->finalize(), canvas->area);
            fascia->handle_events();
#endif
            opus->render();
#endif
#ifdef QUETZAL
            quetzal->send(mv(lattice));
#endif
        }
    }
};

uptr<Euclid> euclid;

void tick() {
    euclid->activate();
}

void bootstrap() {
    spdlog::info("( ) Initializing euclid");
    euclid = mkuptr<Euclid>();

    spdlog::info("( ) dimensions");
#ifdef WASM
    render_width = EM_ASM_INT(return screen.width);
    render_height = EM_ASM_INT(return screen.height);
#else
    render_width = COMPILED_RENDER_WIDTH;
    render_height = COMPILED_RENDER_HEIGHT;
#endif
    spdlog::info("render_width {}, render_height: {}", render_width, render_height);
    spdlog::info("(~) dimensions");

    spdlog::info("( ) acoustics");
#ifdef WASM
    float gain = 1.0;
#endif
#ifdef MAC
    float gain = 1.0;
#endif
#ifdef QUETZAL
    float gain = 0.1;
#endif
#ifdef SDL_AUDIO
    SDL_Init(SDL_INIT_AUDIO);
    audio_input = mkuptr<SDLAudioInput>(FRAME_SIZE);
#endif
#ifdef PORT_AUDIO
    audio_input = mkuptr<PortaudioInput>(FRAME_SIZE);
#endif
    equalizer = mkuptr<Equalizer>(gain);
    fourier_transform = mkuptr<FourierTransform>();
    spdlog::info("(~) acoustics");

    spdlog::info("( ) cosmos");
    cosmos.push_back(mkuptr<Cosmology>(render_width, render_height, STFT_SIZE, Impressions::watercolor));
    spdlog::info("(~) cosmos");

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
    opus = mkuptr<Opus>();
#endif
#ifdef MAC
    fascia = mkuptr<Fascia>(*equalizer);
#endif
#ifdef QUETZAL
    quetzal = mkuptr<Quetzal>();
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
