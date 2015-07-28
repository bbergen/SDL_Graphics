//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_COMMON_H
#define SDL_GRAPHICS_COMMON_H

#define true 1
#define false 0

#if DEBUG
#define assert(expression) if(!(expression)) {*(int*) 0 = 0;}
#else
#define assert(expression)
#endif

#endif //SDL_GRAPHICS_COMMON_H
