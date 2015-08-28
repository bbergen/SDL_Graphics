//
// Created by bryan on 8/27/15.
//

#include "ship.h"

typedef struct _ship {
    int x;
    int y;
} _ship;

typedef struct _bullet {
    int x;
    int y;
} _bullet;

ship
allocate_ship(int x, int y) {
    _ship *s = malloc(sizeof(_ship));
    s->x = x;
    s->x = y;
    return s;
}

void
free_ship(ship s) {
    free(s);
}