//
// Created by bryan on 9/25/15.
//

#include "asteroid_types.h"

#ifndef SDL_GRAPHICS_STARFIELD_H
#define SDL_GRAPHICS_STARFIELD_H

#endif //SDL_GRAPHICS_STARFIELD_H

typedef void* star_field;

star_field init_star_field(screen);

void free_star_field(star_field);

void render_star_field(SDL_Renderer*, star_field);