//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_ASTEROIDS_H
#define SDL_GRAPHICS_ASTEROIDS_H

#include <map.h>
#include "ship.h"
#include "asteroid_types.h"

#define FREE_CHANNEL -1

/*
 *  Global Constants
 */
global const int SCREEN_WIDTH = 1360;
global const int SCREEN_HEIGHT = 768;
global const char* FONT_ASTEROIDS_TITLE = "resources/fonts/over_there.ttf";
global const char* FONT_ASTEROIDS_MENU = "resources/fonts/built_tiling.ttf";
global const char* SOUND_SHIP_THRUSTER = "resources/sounds/asteroids/effects/thruster.wav";
global const char* SOUND_SHIP_SHOOT = "resources/sounds/asteroids/effects/shoot.wav";

/*
 *  Maintains the current state of the game
 */
typedef struct asteroids_game {
    int8_t running;
    SDL_Event *event;
    keyboard *keys;
    screen *scrn;
    ship current_ship;
    hash_map sounds;
} asteroids_game;


/*
 *  Function ProtoTypes
 */
internal void shutdown(void);
internal void free_game(asteroids_game*);

#endif //SDL_GRAPHICS_ASTEROIDS_H
