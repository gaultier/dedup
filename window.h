#pragma once

#include <errno.h>

#include "SDL2/SDL.h"
#include "utils.h"

void window_create(SDL_Window** window) {
    *window = SDL_CreateWindow("dedup", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, 800, 600,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!*window) DIE(EINVAL, "Could not create window: %s\n", SDL_GetError());
}
