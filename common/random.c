//
// Created by bryan on 9/10/15.
//

#include <common.h>
#include <stdlib.h>
#include <time.h>
#include "random.h"

global int8_t random_init = false;

internal void
init_random(void) {
    if (!random_init) {
        srand((uint32_t) time(NULL));
        random_init = true;
    }
}

long
random_in_range(long min, long max) {
    assert(max >= 0 && max <= RAND_MAX);
    long range = max - min;
    if (!random_init) {
        init_random();
    }
    unsigned long num_bins = (unsigned long)  range + 1;
    unsigned long num_rand = (unsigned long) RAND_MAX + 1;
    unsigned long bin_size = num_rand / num_bins;
    unsigned long defect = num_rand % num_bins;

    long x;
    do {
        x = random();
    } while (num_rand - defect <= (unsigned long) x);

    return (x / bin_size) + min;
}

int8_t
random_bool(void) {
    if (!random_init) {
        init_random();
    }
    return random_in_range(0, RAND_MAX) % 2 == 0;
}

double
random_double(void) {
    if (!random_init) {
        init_random();
    }
    double x = ((double) rand() / (double) RAND_MAX);
    return x;
}