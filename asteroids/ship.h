//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_SHIP_H
#define SDL_GRAPHICS_SHIP_H

#include <SDL2/SDL_render.h>
#include <vector.h>
#include "asteroid_types.h"

/*
 * Hidden implementation
 */
typedef void* ship;

/*
 * Hidden implementation
 */
typedef void* bullet;

/*
 * Callback function called when the ship shoots
 */
typedef void (*on_shoot_function)(void*);

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
ship allocate_ship(int x, int y, on_shoot_function shoot, void*);

/*
 * Frees memory associated with a ship
 */
void free_ship(ship s);

/*
 * Queries the thrusting state of the ship
 */
int8_t ship_thrusting(ship s);

/*
 * Sets the state of the ships thrusting sound
 */
void thrusting_state(ship s, int8_t state);

/*
 * Returns a vector containing all visible bullets
 */
vector visible_bullets(ship s);

/*
 * Marks a bullet to be removed
 */
void remove_bullet(bullet b);

/*
 * X Y location of the bullet
 */
point bullet_location(bullet b);

#endif //SDL_GRAPHICS_SHIP_H
