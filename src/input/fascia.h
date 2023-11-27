#pragma once

#include "paradigm.h"
#include "optics/opus.h"
#include <SDL2/SDL_ttf.h>

namespace euclid {

class Fascia : public Name {
private:
    TTF_Font *font;

public:
    Fascia();

    ~Fascia();

    up<Canvas> observe();

    void draw_text(Canvas &canvas, std::string &text, SDL_Color color, Coordinate origin);
};

}
