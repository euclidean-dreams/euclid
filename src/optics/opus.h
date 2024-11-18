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


class TextureCache : public Name {
private:
    SDL_Texture *load_image(std::string path);

    SDL_Texture *generate_circle();

    SDL_Texture *generate_dragon_curve(int magnitude);

public:
    SDL_Texture *circle;
    vect<SDL_Texture *>dragon_curves;

    TextureCache();

    ~TextureCache();
};


class Tesselation : public Name {
public:
    int width;
    int height;
    SDL_Rect area;
    SDL_Texture *texture;

    Tesselation(Lattice &lattice, TextureCache &texture_cache);

    SDL_Texture *finalize();
};

class Opus : public Name {
public:
    void fill(Color color);

    void blit(SDL_Texture *texture, SDL_Rect destination);

    void render();
};

}
