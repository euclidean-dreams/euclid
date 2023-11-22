#pragma once

#include "paradigm.h"

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
        SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void paint_point(Point point, Color color) {
        SDL_SetRenderTarget(renderer, texture);
        SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 255);
        SDL_RenderDrawPoint(renderer, point.x, point.y);
    }

    SDL_Texture *finalize() {
        return texture;
    }

    static up<Canvas> from_lattice(Lattice &lattice) {
        auto canvas = mkup<Canvas>(lattice.width, lattice.height);
        for (int y = 0; y < lattice.height; y++) {
            for (int x = 0; x < lattice.width; x++) {
                Point point{scast<float>(x), scast<float>(y)};
                canvas->paint_point(point, lattice.get_color(x, y));
            }
        }
        return canvas;
    }
};

}
