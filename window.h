#pragma once

#include <SDL2/SDL_ttf.h>
#include <errno.h>

#include "SDL2/SDL.h"
#include "utils.h"

void window_create(SDL_Window** window, SDL_Renderer** renderer) {
    *window = SDL_CreateWindow("dedup", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, 800, 600,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!*window) DIE(EINVAL, "Could not create window: %s\n", SDL_GetError());

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer)
        DIE(EINVAL, "Could not create renderer: %s\n", SDL_GetError());

    if (SDL_SetRenderDrawColor(*renderer, 40, 40, 40, 40) == -1)
        DIE(EINVAL, "Rendering error: %s\n", SDL_GetError());
}

SDL_Texture* window_text(SDL_Renderer* renderer, const char* text,
                         TTF_Font* font, SDL_Color* color, SDL_Rect* rect) {
    SDL_Surface* surface_text = TTF_RenderText_Solid(font, text, *color);
    if (!surface_text)
        DIE(EINVAL, "Could not create text surface: %s\n", SDL_GetError());

    SDL_Texture* texture_text =
        SDL_CreateTextureFromSurface(renderer, surface_text);
    if (!texture_text)
        DIE(EINVAL, "Could not create text texture: %s\n", SDL_GetError());

    if (SDL_RenderCopy(renderer, texture_text, NULL, rect) == -1)
        DIE(EINVAL, "Rendering error: %s\n", SDL_GetError());

    SDL_FreeSurface(surface_text);
    return texture_text;
}
