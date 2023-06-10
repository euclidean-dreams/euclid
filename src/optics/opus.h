#pragma once

#include "axioms.h"

namespace PROJECT_NAMESPACE {

class Opus {
public:
    void blit(SDL_Texture *texture, SDL_Rect destination) {
        SDL_RenderCopy(renderer, texture, nullptr, &destination);
        SDL_DestroyTexture(texture);
    }

    void render() {
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);
    }
};


class Canvas : public Name {
public:
    int width;
    int height;
    SDL_Texture *texture;

    Canvas(int width, int height) :
            width{width},
            height{height},
            texture{} {
        texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                width,
                height
        );
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_Rect entire_texture;
        entire_texture.x = 0;
        entire_texture.y = 0;
        entire_texture.w = width;
        entire_texture.h = height;
        paint_rect(entire_texture, {0, 0, 0, 255});
    }

    void paint_rect(SDL_Rect &rect, Color color) {
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderTarget(renderer, nullptr);
    }

    SDL_Texture *finalize() {
        return texture;
    }
};

}
