//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_SHIP_H
#define SDL_GRAPHICS_SHIP_H

#include <SDL2/SDL_render.h>
#include "asteroid_types.h"

/*
 * Hidden implementation
 */
typedef void* ship;

/*
 * Updates the state and location of the player's ship on the screen
 */
void update_ship(ship s, keyboard keys, screen scrn);

/*
 * Draws a ship struct to the screen
 */
void render_ship(SDL_Renderer *renderer, ship s);

/*
 * Allocates memory for a ship
 */
ship allocate_ship(int x, int y);

/*
 * Frees memory associated with a ship
 */
void free_ship(ship s);

/*
 * Queries the thrusting state of the ship
 */
int8_t ship_thrusting(ship s);

#endif //SDL_GRAPHICS_SHIP_H
