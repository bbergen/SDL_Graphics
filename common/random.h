//
// Created by bryan on 9/10/15.
//

#ifndef SDL_GRAPHICS_RANDOM_H
#define SDL_GRAPHICS_RANDOM_H

#include <stdint.h>

/*
 * Generates a pseudo-random, uniform number no smaller that the first arg
 * and no larger than the second arg (inclusive)
 */
long random_in_range(long, long);

/*
 * Generates a pseudo-random boolean value
 */
int8_t random_bool(void);

/*
 * Generates a pseudo-random boolean value
 * with a probability of being true equal
 * to (the passed double * 100)%
 */
int8_t random_bool_with_probability(double);

/*
 * Generates a pseudo-random, uniform double between 0 and 1.0000
 */
double random_double(void);

#endif //SDL_GRAPHICS_RANDOM_H
