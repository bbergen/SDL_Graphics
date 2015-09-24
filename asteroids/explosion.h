//
// Created by bryan on 8/27/15.
//

#include <SDL2/SDL_render.h>
#include "asteroid_types.h"

#ifndef SDL_GRAPHICS_EXPLOSION_H
#define SDL_GRAPHICS_EXPLOSION_H

/*
 * hidden implementation
 */
typedef void* explosion;

/*
 * Update the size and location of an explosion
 */
int8_t update_explosion(explosion e);

/*
 * Draw an explosion to the screen
 * Pass renderer as void start for list compatibility
 */
int8_t render_explosion(explosion e, void *r);

/*
 * Create an explosion
 */
explosion generate_explosion(int x, int y, screen scrn);

/*
 * Free explosion memory
 */
void free_explosion(explosion e);

/*
 * Returns the size of an explosion struct
 */
size_t explosion_size(void);

#endif //SDL_GRAPHICS_EXPLOSION_H
