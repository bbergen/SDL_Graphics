//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_ASTEROIDS_H
#define SDL_GRAPHICS_ASTEROIDS_H

#include "ship.h"

#define SCREEN_WIDTH 1360
#define SCREEN_HEIGHT 768

typedef struct keyboard {
    int8_t left_down;
    int8_t right_down;
    int8_t up_down;
    int8_t down_down;
} keyboard;

typedef struct asteroids_game {
    int8_t running;
    SDL_Event *event;
    keyboard *keys;
    ship current_ship;
} asteroids_game;

#endif //SDL_GRAPHICS_ASTEROIDS_H
