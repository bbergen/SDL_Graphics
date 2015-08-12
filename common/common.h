//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_COMMON_H
#define SDL_GRAPHICS_COMMON_H

#define true 1
#define false 0
#define PI 3.14159265359
#define count(array) (sizeof(array) / sizeof((array)[0]))

#define persistent static
#define internal static
#define global static

#if DEBUG
#define assert(expression) if(!(expression)) {*(int*) 0 = 0;}
#else
#define assert(expression)
#endif

#endif //SDL_GRAPHICS_COMMON_H
