#pragma once

#include "foundation.h"

namespace PROJECT_NAMESPACE {

class Opus {
private:
    int width = 512;
    int height = 512;
    SDL_Window *window;
    SDL_Renderer *renderer;

public:
    Opus() {
        auto flags = 0;
        SDL_CreateWindowAndRenderer(width, height, flags, &window, &renderer);
    }

    ~Opus() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }

    void render(SDL_Texture *texture) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }


    SDL_Texture *create_texture() {
        Uint32 red_mask, green_mask, blue_mask, alpha_mask;
        red_mask = 0x000000ff;
        green_mask = 0x0000ff00;
        blue_mask = 0x00ff0000;
        alpha_mask = 0xff000000;
        auto surface = SDL_CreateRGBSurface(0, width, height, 32, red_mask, green_mask, blue_mask, alpha_mask);
        if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
        auto surface_pixels = (Uint32 *) surface->pixels;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                auto pixelIndex = width * y + x;
                surface_pixels[pixelIndex] = Pixel{0, scast<uint8_t>(get_current_time() % 255), 200, 255}.serialize();
            }
        }
        auto texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_UnlockSurface(surface);
        SDL_FreeSurface(surface);
        return texture;
    }
};

}
