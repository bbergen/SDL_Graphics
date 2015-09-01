//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_ASTEROIDS_H
#define SDL_GRAPHICS_ASTEROIDS_H

#include "ship.h"
#include "asteroid_types.h"

/*
 *  Global Constants
 */
global const int SCREEN_WIDTH = 1360;
global const int SCREEN_HEIGHT = 768;
global const char* ASTEROIDS_TITLE_FONT = "resources/fonts/over_there.ttf";
global const char* ASTEROIDS_MENU_FONT = "resources/fonts/built_tiling.ttf";

/*
 *  Maintains the current state of the game
 */
typedef struct asteroids_game {
    int8_t running;
    SDL_Event *event;
    keyboard *keys;
    screen *scrn;
    ship current_ship;
} asteroids_game;


/*
 *  Function ProtoTypes
 */
internal void shutdown(void);
internal void free_game(asteroids_game*);

#endif //SDL_GRAPHICS_ASTEROIDS_H
