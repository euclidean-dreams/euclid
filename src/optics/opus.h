#pragma once

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

    SDL_Texture *finalize();
};

class Tesselation : public Name {
public:
    int width;
    int height;
    SDL_Rect area;
    SDL_Texture *texture;

    Tesselation(Lattice &lattice);

    SDL_Texture *generate_circle();

    SDL_Texture *load_image(std::string path);

    SDL_Texture *finalize();
};

class Opus : public Name {
public:
    void fill(Color color);

    void blit(SDL_Texture *texture, SDL_Rect destination);

    void render();
};

}
