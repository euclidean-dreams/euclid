#pragma once

#include "foundation.h"

namespace PROJECT_NAMESPACE {

class Opus {
    int width;
    int height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

public:
    Opus(int width, int height) : width{width}, height{height} {
        SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
        window = SDL_CreateWindow("euclid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height,
                                  SDL_WINDOW_BORDERLESS);
        Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
        renderer = SDL_CreateRenderer(window, -1, rendererFlags);
        texture = SDL_CreateTexture(
                renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height
        );
    }

    ~Opus() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }

    void render(up<Signal<Pixel>> signal) {
        void *pixel_buffer;
        int pitch = scast<int>(width * sizeof(Pixel));
        SDL_LockTexture(texture, nullptr, &pixel_buffer, &pitch);
        memcpy(pixel_buffer, signal->get_data_pointer(), signal->size() * sizeof(Pixel));
        SDL_UnlockTexture(texture);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
};

}
