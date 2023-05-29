#pragma once

#include "foundation.h"

namespace PROJECT_NAMESPACE {

class Opus {
public:
    void render(SDL_Texture *texture, SDL_Rect destination) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, &destination);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
    }
};

}
