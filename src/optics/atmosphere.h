#pragma once

#include "axioms.h"
#include "cosmology/psyche.h"
#include "opus.h"

namespace PROJECT_NAMESPACE {

class Atmosphere : public Name {
private:
    int width;
    int height;
    up<Harmony> harmony;

public:
    Atmosphere(int width, int height, up<Harmony> harmony)
            : width{width},
              height{height},
              harmony{mv(harmony)} {

    }

    SDL_Texture *observe() {
        Canvas canvas{width, height};

        return canvas.finalize();
    }
};

}
