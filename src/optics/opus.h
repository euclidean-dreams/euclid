#pragma once

#include <SDL2/SDL_ttf.h>
#include "paradigm.h"

namespace euclid {

extern SDL_Window *window;
extern SDL_Renderer *renderer;

class Canvas : public Name {
public:
    int width;
    int height;
    SDL_Rect area;
    SDL_Texture *texture;

    Canvas(int width, int height);

    Canvas(Lattice &lattice);

    void paint_rect(SDL_Rect &rect, Color color);

    void paint_point(Point point, Color color);

    SDL_Texture *finalize();
};

class Fascia : public Name {
private:
    TTF_Font *font;

public:
    Fascia();

    ~Fascia();

    up<Canvas> observe();

    void draw_text(Canvas &canvas, std::string &text, SDL_Color color, Coordinate origin);
};

class Opus : public Name {
private:
    up<Fascia> fascia;

public:
    Opus() : fascia{mkup<Fascia>()} {}

    void blit(SDL_Texture *texture, SDL_Rect destination);

    void render();
};

}
