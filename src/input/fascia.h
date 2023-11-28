#pragma once

#include "paradigm.h"
#include "optics/opus.h"
#include "acoustics/equalizer.h"
#include <SDL2/SDL_ttf.h>

namespace euclid {

class Fascia : public Name {
private:
    Equalizer &equalizer;
    TTF_Font *font;

    void draw_text(Canvas &canvas, std::string &text, SDL_Color color, Coordinate origin);

public:
    Fascia(Equalizer &equalizer);

    ~Fascia();

    void handle_events();

    up<Canvas> observe();
};

}
