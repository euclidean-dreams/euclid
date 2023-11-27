#include "fascia.h"

namespace euclid {

Fascia::Fascia() {
    TTF_Init();
    font = TTF_OpenFont("./assets/DMSans-Bold.ttf", 33);
}

Fascia::~Fascia() {
    TTF_Quit();
    TTF_CloseFont(font);
}

up<Canvas> Fascia::observe() {
    auto canvas = mkup<Canvas>(render_width, render_height);
    auto horizontal_offset = render_width / 100;
    auto vertical_offset = render_height / 30;

    std::string excitation_label = "excitation ~ " + std::to_string(COSMOLOGY_EXCITATION);
    draw_text(*canvas, excitation_label, {111, 133, 199}, {horizontal_offset, vertical_offset});

    std::string excitation_decrease_label = "<";
    Coordinate excitation_decrease_position{horizontal_offset * 2, vertical_offset * 2};
    draw_text(*canvas, excitation_decrease_label, {111, 133, 199}, excitation_decrease_position);

    std::string excitation_increase_label = ">";
    Coordinate excitation_increase_position{horizontal_offset * 9, vertical_offset * 2};
    draw_text(*canvas, excitation_increase_label, {111, 133, 199}, excitation_increase_position);
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
