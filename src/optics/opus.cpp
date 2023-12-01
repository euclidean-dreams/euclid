#include "opus.h"

namespace euclid {

Canvas::Canvas(int width, int height) :
        width{width},
        height{height},
        area{0, 0, width, height},
        texture{} {
    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
    );
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

Canvas::Canvas(Lattice &lattice) :
        width{lattice.width},
        height{lattice.height},
        area{0, 0, width, height},
        texture{} {
    Uint32 redMask, greenMask, blueMask, alphaMask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    redMask = 0xff000000;
    greenMask = 0x00ff0000;
    blueMask = 0x0000ff00;
    alphaMask = 0x000000ff;
#else
    redMask = 0x000000ff;
    greenMask = 0x0000ff00;
    blueMask = 0x00ff0000;
    alphaMask = 0xff000000;
#endif
    auto surface = SDL_CreateRGBSurface(0, render_width, render_height, 32,
                                        redMask, greenMask, blueMask, alphaMask);
    auto pixels = (Uint32 *) surface->pixels;
    memset(pixels, 0, render_width * render_height * sizeof(Uint32));
    for (int i = 0; i < render_width * render_height; i++) {
        pixels[i] = SDL_MapRGBA(surface->format, 0, 0, 0, 255);
    }
    for (auto &dot: lattice) {
        auto coordinate = dot.first;
        auto color = dot.second;
        auto surface_color = SDL_MapRGBA(surface->format, color.red, color.green, color.blue, 255);
        auto pixel_index = render_width * (render_height - 1 - coordinate.y) + coordinate.x;
        pixels[pixel_index] = surface_color;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
}

void Canvas::paint_rect(SDL_Rect &rect, Color color) {
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Canvas::paint_point(Point point, Color color) {
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 255);
    SDL_RenderDrawPoint(renderer, point.x, point.y);
}

SDL_Texture *Canvas::finalize() {
    return texture;
}


void Opus::blit(SDL_Texture *texture, SDL_Rect destination) {
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderCopy(renderer, texture, nullptr, &destination);
    SDL_DestroyTexture(texture);
}

void Opus::render() {
    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderPresent(renderer);
}

}
