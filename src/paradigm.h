#pragma once

#include <spdlog/spdlog.h>
#include "axioms.h"

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <SDL2/SDL.h>

#else

#include <SDL.h>

#endif

#define PROJECT_NAMESPACE euclid

using namespace cosmology;

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
