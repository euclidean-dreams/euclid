#pragma once

#include <spdlog/spdlog.h>
#include "axioms.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

using namespace cosmology;

///////////////////
////// constants
///////////
namespace euclid {

extern int FRAME_SIZE;
extern int render_width;
extern int render_height;

#ifdef OPUS
SDL_Window *window;
SDL_Renderer *renderer;
#endif

}
