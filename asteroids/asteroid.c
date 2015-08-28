//
// Created by bryan on 8/27/15.
//

#include "asteroid.h"

#define MAX_VERTICES 10

typedef struct _asteroid {
    ASTEROID_TYPE type;
    int min_x;
    int min_y;
    int max_x;
    int max_y;
    float anchor_x;
    float anchor_y;
    int vertices;
    int x[MAX_VERTICES];
    int y[MAX_VERTICES];
} _asteroid;