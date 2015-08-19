//
// Created by bryan on 8/1/15.
//

#ifndef SDL_GRAPHICS_UTIL_H
#define SDL_GRAPHICS_UTIL_H

/*
 * converts an integer to a string
 * if 3rd param is > 0, string will be padded with leading zeros
 */
void itoa(int, char[], int);

/*
 * Generic error function designed to take a pointer to and print the
 * results of any of the various SDL error functions
 */
void error(const char*(*error_function)(void));

#endif //SDL_GRAPHICS_UTIL_H
