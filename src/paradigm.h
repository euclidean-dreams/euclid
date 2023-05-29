#pragma once

#include <vector>
#include <list>
#include <spdlog/spdlog.h>

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <SDL2/SDL.h>

#else

#include <SDL.h>

#endif

#define PROJECT_NAMESPACE euclid

////////////////////////
/////////// helpful macros
/////////////////
#define up std::unique_ptr
#define mkup std::make_unique
#define sp std::shared_ptr
#define mksp std::make_shared
#define mv std::move
#define vec std::vector
#define lst std::list
#define umap std::unordered_map
#define scast static_cast
#define cmplx std::complex<double>

///////////////////
////// constants
///////////
namespace PROJECT_NAMESPACE {

extern int FRAME_SIZE;
extern int render_width;
extern int render_height;

SDL_Window *window;
SDL_Renderer *renderer;

}
