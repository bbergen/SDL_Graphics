//
// Created by bryan on 8/27/15.
//

#ifndef SDL_GRAPHICS_ASTEROID_H
#define SDL_GRAPHICS_ASTEROID_H

#include <SDL2/SDL_render.h>
#include "asteroid_types.h"

typedef enum ASTEROID_TYPE {
    LARGE, SMALL
} ASTEROID_TYPE;

/*
 * Hidden implementation
 */
typedef void* asteroid;

/*
 * Updates location of asteroid on the screen
 */
void update_asteroid(asteroid a, screen scrn);

/*
 * Draws an asteroid to the screen
 */
void render_asteroid(SDL_Renderer *renderer, asteroid a);

/*
 * Creates an asteroid of the specific type at the specific location
 */
asteroid generate_asteroid(int x, int y, screen scrn, ASTEROID_TYPE type);

/*
 * Frees asteroid memory
 */
void free_asteroid(asteroid a);

/*
 * Checks if the asteroid polygon contains the passed point
 */
int8_t asteroid_contains(asteroid a, point p);

void explode(asteroid a);

/*
 * Returns the type of the passed asteroid
 */
ASTEROID_TYPE asteroid_type(asteroid a);

/*
 * Returns the anchor of the asteroid in point form
 */
point asteroid_location(asteroid a);

int8_t asteroid_visible(asteroid a);

#endif //SDL_GRAPHICS_ASTEROID_H
