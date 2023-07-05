#pragma once

#include "axioms.h"

namespace PROJECT_NAMESPACE {

class Opus {
public:
    void blit(SDL_Texture *texture, SDL_Rect destination) {
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, texture, nullptr, &destination);
        SDL_DestroyTexture(texture);
    }

    void render() {
        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderPresent(renderer);
    }
};


class Canvas : public Name {
public:
    int width;
    int height;
    SDL_Texture *texture;
    SDL_Rect area;

    Canvas(int width, int height) :
            width{width},
            height{height},
            texture{},
            area{0, 0, width, height} {
        texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_TARGET,
                width,
                height
        );
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    void paint_rect(SDL_Rect &rect, Color color) {
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
        SDL_RenderFillRect(renderer, &rect);
    }

    void paint_point(Point point, Color color) {
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, color.alpha);
        SDL_RenderDrawPoint(renderer, point.x, point.y);
    }

    SDL_Texture *finalize() {
        return texture;
    }
};

}
