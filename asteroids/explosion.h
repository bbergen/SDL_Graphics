//
// Created by bryan on 8/27/15.
//

#include <SDL2/SDL_render.h>

#ifndef SDL_GRAPHICS_EXPLOSION_H
#define SDL_GRAPHICS_EXPLOSION_H

/*
 * hidden implementation
 */
typedef void* explosion;

/*
 * Update the size and location of an explosion
 */
void update_explosion(explosion e);

/*
 * Draw an explosion to the screen
 */
void render_explosion(SDL_Renderer *renderer, explosion e);

/*
 * Create an explosion
 */
explosion allocate_explosion(int x, int y, int radius);

/*
 * Free explosion memory
 */
void free_explosion(explosion e);

#endif //SDL_GRAPHICS_EXPLOSION_H
