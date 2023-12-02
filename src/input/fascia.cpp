#include "fascia.h"

namespace euclid {

Fascia::Fascia(Equalizer &equalizer) : equalizer{equalizer} {
    TTF_Init();
    font = TTF_OpenFont("./assets/DMSans-Bold.ttf", 25);
}

Fascia::~Fascia() {
    TTF_Quit();
    TTF_CloseFont(font);
}

void Fascia::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            auto symbol = event.key.keysym.sym;
            auto keymod = event.key.keysym.mod;
            auto shift_held_down = SDLK_LSHIFT & keymod;
            auto multiplier = 1;
            if (shift_held_down) {
                multiplier = 10;
            }
            if (symbol == SDLK_SPACE) {
                display_fascia = !display_fascia;
            } else if (symbol == SDLK_q) {
                equalizer.nudge_gain(0.1 * multiplier);
            } else if (symbol == SDLK_a) {
                equalizer.nudge_gain(-0.1 * multiplier);
            } else if (symbol == SDLK_w) {
                MOVEMENT = embind_flt(0.01, MOVEMENT + 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_s) {
                MOVEMENT = embind_flt(0.01, MOVEMENT - 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_e) {
                MAGNITUDE = embind_flt(0.01, MAGNITUDE + 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_d) {
                MAGNITUDE = embind_flt(0.01, MAGNITUDE - 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_r) {
                RESONANCE = embind_flt(0.01, RESONANCE + 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_f) {
                RESONANCE = embind_flt(0.01, RESONANCE - 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_t) {
                TWIST = embind_flt(0.01, TWIST + 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_g) {
                TWIST = embind_flt(0.01, TWIST - 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_y) {
                CHAOS = embind_flt(0.01, CHAOS + 0.1 * multiplier, 99999);
            } else if (symbol == SDLK_h) {
                CHAOS = embind_flt(0.01, CHAOS - 0.1 * multiplier, 99999);
            }
        }
    }
}

up<Canvas> Fascia::observe() {
    auto canvas = mkup<Canvas>(render_width, render_height);
    if (display_fascia) {
        auto horizontal_offset = render_width / 100;
        auto vertical_offset = render_height / 30;

        std::string label = "[q|a] gain ~ " + std::to_string(equalizer.get_gain());
        draw_text(*canvas, label, {210, 99, 150}, {horizontal_offset, vertical_offset});

        label = "[w|s] movement ~ " + std::to_string(MOVEMENT);
        draw_text(*canvas, label, {210, 99, 150}, {horizontal_offset, vertical_offset * 2});

        label = "[e|d] magnitude ~ " + std::to_string(MAGNITUDE);
        draw_text(*canvas, label, {210, 99, 150}, {horizontal_offset, vertical_offset * 3});

        label = "[r|f] resonance ~ " + std::to_string(RESONANCE);
        draw_text(*canvas, label, {210, 99, 150}, {horizontal_offset, vertical_offset * 4});

        label = "[t|g] twist ~ " + std::to_string(TWIST);
        draw_text(*canvas, label, {210, 99, 150}, {horizontal_offset, vertical_offset * 5});

        label = "[y|h] chaos ~ " + std::to_string(CHAOS);
        draw_text(*canvas, label, {210, 99, 150}, {horizontal_offset, vertical_offset * 6});
    }
    return canvas;
}

void Fascia::draw_text(Canvas &canvas, std::string &text, SDL_Color color, Coordinate origin) {
    auto surface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    auto text_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetRenderTarget(renderer, canvas.texture);
    SDL_Rect destination_area{origin.x, origin.y, surface->w, surface->h};
    SDL_RenderCopy(renderer, text_texture, nullptr, &destination_area);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(text_texture);
}

}
