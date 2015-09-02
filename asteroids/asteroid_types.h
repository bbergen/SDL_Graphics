//
// Created by bryan on 8/31/15.
//

#ifndef SDL_GRAPHICS_ASTEROID_TYPES_H
#define SDL_GRAPHICS_ASTEROID_TYPES_H

#include <stdint.h>

typedef struct keyboard {
    int8_t left_down;
    int8_t right_down;
    int8_t up_down;
    int8_t down_down;
} keyboard;

typedef struct screen {
    int width;
    int height;
} screen;

typedef struct point {
    int x;
    int y;
} point;

#endif //SDL_GRAPHICS_ASTEROID_TYPES_H
