#include "opus.h"
#include <SDL2/SDL_image.h>

namespace euclid {

enum DragonCurveRunology {
    advance_f,
    advance_g,
    rotate_left_quarter,
    rotate_right_quarter
};

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

Tesselation::Tesselation(Lattice &lattice, TextureCache &texture_cache) :
        width{lattice.width},
        height{lattice.height},
        area{0, 0, width, height},
        texture{} {
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, texture);
    SDL_RenderClear(renderer);
    for (auto &dot: lattice) {
        auto coordinate = dot.first;
        auto pith = dot.second;
        auto color = pith.color;
        auto rotation = pith.twist * 180 / M_PI;
        auto scale = pith.magnitude;
        int dest_top_left_x = coordinate.x - scale / 2;
        int dest_top_left_y = coordinate.y - scale / 2;
        SDL_Texture *tessellation_texture;
        if (pith.tesselation == 0) {
            tessellation_texture = texture_cache.circle;
        } else if (pith.tesselation == 1) {
            auto dragon_curve_index = pith.magnitude / 3;
            if (dragon_curve_index > texture_cache.dragon_curves.size() - 1) {
                dragon_curve_index = texture_cache.dragon_curves.size() - 1;
            }
            tessellation_texture = texture_cache.dragon_curves[dragon_curve_index];
        } else {
            tessellation_texture = texture_cache.circle;
        }
        SDL_Rect destination_area{dest_top_left_x, dest_top_left_y, scint(scale), scint(scale)};
        SDL_SetTextureColorMod(tessellation_texture, color.red, color.green, color.blue);
        SDL_RenderCopyEx(renderer, tessellation_texture, nullptr, &destination_area,
                         rotation, nullptr, SDL_FLIP_NONE);
    }
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


TextureCache::TextureCache() :
        circle{generate_circle()},
        dragon_curves{} {
    for (int index = 0; index < 16; index++) {
        dragon_curves.push_back(generate_dragon_curve(index));
    }
}


SDL_Texture *TextureCache::load_image(std::string path) {
    auto surface = IMG_Load(path.c_str());
    auto image_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return image_texture;
}


SDL_Texture *TextureCache::generate_circle() {
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

SDL_Texture *TextureCache::generate_dragon_curve(int magnitude) {
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
    int size = 1024;
    auto surface = SDL_CreateRGBSurface(0, size, size, 32,
                                        red_mask, green_mask, blue_mask, alpha_mask);
    auto pixels = (Uint32 *) surface->pixels;

    Point turtle_position = {scflt(size) / 2, scflt(size) / 2};
    float turtle_rotation = 0;
    uptr<vect<DragonCurveRunology>> state = mkuptr<vect<DragonCurveRunology>>();
    state->push_back(DragonCurveRunology::advance_f);
    state->push_back(DragonCurveRunology::advance_g);
    for (int step = 0; step < magnitude; step++) {
        uptr<vect<DragonCurveRunology>> next_state = mkuptr<vect<DragonCurveRunology>>();;
        for (auto rune: *state) {
            if (rune == DragonCurveRunology::advance_f) {
                next_state->push_back(DragonCurveRunology::advance_f);
                next_state->push_back(DragonCurveRunology::rotate_left_quarter);
                next_state->push_back(DragonCurveRunology::advance_g);
            } else if (rune == DragonCurveRunology::advance_g) {
                next_state->push_back(DragonCurveRunology::advance_f);
                next_state->push_back(DragonCurveRunology::rotate_right_quarter);
                next_state->push_back(DragonCurveRunology::advance_g);
            } else if (rune == DragonCurveRunology::rotate_left_quarter) {
                next_state->push_back(DragonCurveRunology::rotate_left_quarter);
            } else if (rune == DragonCurveRunology::rotate_right_quarter) {
                next_state->push_back(DragonCurveRunology::rotate_right_quarter);
            }
        }
        state = mv(next_state);
    }
    for (int index = 0; index < state->size(); index++) {
        auto rune = (*state)[index];
        if (rune == DragonCurveRunology::advance_f || rune == DragonCurveRunology::advance_g) {
            turtle_position = Point::from_polar(turtle_position, 1, turtle_rotation);
            auto pixel_index = size * (size - 1 - scint(turtle_position.y)) + scint(turtle_position.x);
            auto fader = 128 * index / state->size();
            auto surface_color = SDL_MapRGBA(surface->format, 255, 255, 255, 255 - fader);
            pixels[pixel_index] = surface_color;
        } else if (rune == DragonCurveRunology::rotate_left_quarter) {
            turtle_rotation += M_PI / 2;
        } else if (rune == DragonCurveRunology::rotate_right_quarter) {
            turtle_rotation -= M_PI / 2;
        }
    }
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

TextureCache::~TextureCache() {
    SDL_DestroyTexture(circle);
}

}
