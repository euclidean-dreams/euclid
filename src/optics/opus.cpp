#include "opus.h"
#include <SDL2/SDL_image.h>

namespace euclid {

Canvas::Canvas(int width, int height) :
        width{width},
        height{height},
        area{0, 0, width, height},
        texture{} {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
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
        auto null_color = lattice.null_pith.color;
        pixels[i] = SDL_MapRGBA(surface->format, null_color.red, null_color.green, null_color.blue, 0);
    }
    for (auto &dot: lattice) {
        auto coordinate = dot.first;
        auto pith = dot.second;
        auto color = pith.color;
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

SDL_Texture *Canvas::finalize() {
    return texture;
}

Tesselation::Tesselation(Lattice &lattice) :
        width{lattice.width},
        height{lattice.height},
        area{0, 0, width, height},
        texture{} {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderClear(renderer);
    auto tessellation_texture = generate_circle();
    for (auto &dot: lattice) {
        auto coordinate = dot.first;
        auto pith = dot.second;
        auto color = pith.color;
        auto rotation = pith.twist * 180 / M_PI;
        int dest_top_left_x = coordinate.x - pith.magnitude / 2;
        int dest_top_left_y = coordinate.y - pith.magnitude / 2;
        SDL_Rect destination_area{dest_top_left_x, dest_top_left_y, scint(pith.magnitude), scint(pith.magnitude)};
        SDL_SetTextureColorMod(tessellation_texture, color.red, color.green, color.blue);
        SDL_RenderCopyEx(renderer, tessellation_texture, nullptr, &destination_area,
                         rotation, nullptr, SDL_FLIP_NONE);
    }
    SDL_DestroyTexture(tessellation_texture);
}

SDL_Texture *Tesselation::generate_circle() {
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
    int size = 128;
    auto surface = SDL_CreateRGBSurface(0, size, size, 32,
                                        red_mask, green_mask, blue_mask, alpha_mask);
    auto pixels = (Uint32 *) surface->pixels;
    auto surface_color = SDL_MapRGBA(surface->format, 255, 255, 255, 128);
    for (int y = 0; y <= size; y++) {
        for (int x = 0; x <= size; x++) {
            auto radius = size / 2;
            if (radius * radius > (x - size / 2) * (x - size / 2) +
                                  (y - size / 2) * (y - size / 2)) {
                auto pixel_index = size * (size - 1 - y) + x;
                pixels[pixel_index] = surface_color;
            }
        }
    }
    auto circle_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return circle_texture;
}

SDL_Texture *Tesselation::load_image(std::string path) {
    auto surface = IMG_Load(path.c_str());
    auto image_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return image_texture;
}

SDL_Texture *Tesselation::finalize() {
    return texture;
}


void Opus::fill(Color color) {
    auto background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, 1, 1);
    SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, background);
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, 255);
    auto fill_area = SDL_Rect{1, 1, 1, 1};
    SDL_RenderFillRect(renderer, &fill_area);

    SDL_SetRenderTarget(renderer, nullptr);
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
    SDL_DestroyTexture(background);
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
