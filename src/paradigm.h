#pragma once

#include <spdlog/spdlog.h>
#include "axioms.h"

#ifndef QUETZAL
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#endif

using namespace cosmology;

///////////////////
////// constants
///////////

namespace euclid {

extern int render_width;
extern int render_height;

extern bool running;

}
