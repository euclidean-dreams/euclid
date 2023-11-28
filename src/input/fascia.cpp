#include "fascia.h"

namespace euclid {

Fascia::Fascia(Equalizer &equalizer) : equalizer{equalizer} {
    TTF_Init();
    font = TTF_OpenFont("./assets/DMSans-Bold.ttf", 33);
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
            if (symbol == SDLK_UP) {
                equalizer.nudge_gain(0.1 * multiplier);
            } else if (symbol == SDLK_DOWN) {
                equalizer.nudge_gain(-0.1 * multiplier);
            } else if (symbol == SDLK_SPACE) {
            }
        }
    }
}

up<Canvas> Fascia::observe() {
    auto canvas = mkup<Canvas>(render_width, render_height);
    auto horizontal_offset = render_width / 100;
    auto vertical_offset = render_height / 30;

    std::string sensitivity_label = "sensitivity ~ " + std::to_string(equalizer.get_gain());
    draw_text(*canvas, sensitivity_label, {111, 133, 199}, {horizontal_offset, vertical_offset});
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
