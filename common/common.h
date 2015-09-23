//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_COMMON_H
#define SDL_GRAPHICS_COMMON_H

#define true 1
#define false 0
#define PI 3.14159265359
#define count(array) (sizeof(array) / sizeof((array)[0]))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define RADIANS (PI * 2)
#define TO_RADIANS(degrees) (degrees) * (PI / 180)
#define TO_DEGREES(radians) (radians) * (180 / PI)

#define persistent static
#define internal static
#define global static

#if DEBUG
#define assert(expression) if(!(expression)) {*(int*) 0 = 0;}
#else
#define assert(expression)
#endif

#endif //SDL_GRAPHICS_COMMON_H
