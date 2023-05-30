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

}
