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

uptr<SDLAudioInput> audio_input;
uptr<Equalizer> equalizer;
uptr<FourierTransform> fourier_transform;
uptr<Cosmology> cosmos;
#ifdef OPUS
uptr<Opus> opus;
uptr<Fascia> fascia;
SDL_Window *window;
SDL_Renderer *renderer;
int canvas_pixel_stretch;
#endif
#ifdef WAVELET
uptr<Wavelet> wavelet;
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
#endif
#ifdef MAC
    render_width = 1470;
    render_height = 956;
#endif
#ifdef WAMULATOR
    render_width = 64 * canvas_pixel_stretch;
    render_height = 64 * canvas_pixel_stretch;
#endif
#ifdef PANTHEON
    render_width = 64;
    render_height = 64;
#endif
    spdlog::info("render_width {}, render_height: {}", render_width, render_height);
    spdlog::info("(~) dimensions");

    spdlog::info("( ) acoustics");
    SDL_Init(SDL_INIT_AUDIO);
    audio_input = mkuptr<SDLAudioInput>(FRAME_SIZE);
    equalizer = mkuptr<Equalizer>();
    fourier_transform = mkuptr<FourierTransform>();
    spdlog::info("(~) acoustics");

    spdlog::info("( ) cosmology");
    auto cosmology_width = render_width;
    auto cosmology_height = render_height;
#ifdef WAMULATOR
    cosmology_width = render_width / canvas_pixel_stretch;
    cosmology_height = render_height / canvas_pixel_stretch;
#endif
    cosmos = mkuptr<Cosmology>(cosmology_width, cosmology_height, STFT_SIZE);
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
    canvas_pixel_stretch = 1;
#ifdef WAMULATOR
    canvas_pixel_stretch = 3;
#endif
    spdlog::info("(~) renderer");
    opus = mkuptr<Opus>();
    fascia = mkuptr<Fascia>(*equalizer);
#endif
#ifdef WAVELET
    wavelet = mkuptr<Wavelet>();
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
