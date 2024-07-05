#pragma once

#include "paradigm.h"
#include "optics/opus.h"
#include "perception/equalizer.h"
#include <SDL2/SDL_ttf.h>

namespace euclid {

class Fascia : public Name {
private:
    Equalizer &equalizer;
    TTF_Font *font;
    bool display_fascia = false;

    void draw_text(Canvas &canvas, std::string &text, SDL_Color color, Coordinate origin);

public:
    Fascia(Equalizer &equalizer);

    ~Fascia();

    void handle_events();

    uptr<Canvas> observe();
};

}
