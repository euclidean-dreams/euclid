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
    Uint32 red_mask, green_mask, blue_mask, alpha_mask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    redMask = 0xff000000;
    greenMask = 0x00ff0000;
    blueMask = 0x0000ff00;
    alphaMask = 0x000000ff;
#else
    red_mask = 0x000000ff;
    green_mask = 0x0000ff00;
    blue_mask = 0x00ff0000;
    alpha_mask = 0xff000000;
#endif
    auto surface = SDL_CreateRGBSurface(0, render_width, render_height, 32,
                                        red_mask, green_mask, blue_mask, alpha_mask);
    auto pixels = (Uint32 *) surface->pixels;
    for (int i = 0; i < render_width * render_height; i++) {
        auto null_color = lattice.null_color;
        pixels[i] = SDL_MapRGBA(surface->format, null_color.red, null_color.green, null_color.blue, 255);
    }
    for (auto &dot: lattice) {
        auto coordinate = dot.first;
        auto color = dot.second;
        auto surface_color = SDL_MapRGBA(surface->format, color.red, color.green, color.blue, 255);
        int stretch = 1;
        auto initial_y = coordinate.y * stretch;
        for (int y = initial_y; y < initial_y + stretch; y++) {
            auto initial_x = coordinate.x * stretch;
            for (int x = initial_x; x < initial_x + stretch; x++) {
                auto pixel_index = render_width * (render_height - 1 - y) + x;
                pixels[pixel_index] = surface_color;
            }
        }
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
