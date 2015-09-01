//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_ASTEROIDS_H
#define SDL_GRAPHICS_ASTEROIDS_H

#include "ship.h"
#include "asteroid_types.h"

#define SCREEN_WIDTH 1360
#define SCREEN_HEIGHT 768

typedef struct asteroids_game {
    int8_t running;
    SDL_Event *event;
    keyboard *keys;
    screen *scrn;
    ship current_ship;
} asteroids_game;

#endif //SDL_GRAPHICS_ASTEROIDS_H
