//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_SHIP_H
#define SDL_GRAPHICS_SHIP_H

#include <SDL2/SDL_render.h>

/*
 * Hidden implementation
 */
typedef void* bullet;

/*
 * Hidden implementation
 */
typedef void* ship;

/*
 * Updates the state and location of the player's ship on the screen
 */
void update_ship(ship s);

/*
 * Updates the state and location of a single bullet on the screen
 */
void update_bullet(bullet b);

/*
 * Draws a ship struct to the screen
 */
void render_ship(SDL_Renderer *renderer, ship s);

/*
 * Draws a single bullet on the screen
 */
void render_bullet(SDL_Renderer *renderer, bullet b);

/*
 * Allocates memory for a ship
 */
ship* allocate_ship(int x, int y);

/*
 * Frees memory associated with a ship
 */
void free_ship(ship s);

/*
 * Allocates a bullet a the specific location
 */
bullet* allocate_bullet(int x, int y, int vector);

/*
 * Frees memory associated with a bullet
 */
void free_bullet(bullet b);

#endif //SDL_GRAPHICS_SHIP_H
