//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <random.h>
#include <colors.h>
#include "asteroid.h"

#define MAX_VERTICES 12

typedef struct _asteroid {
    ASTEROID_TYPE type;
    float anchor_x;
    float anchor_y;
    int vertices;
    int *x;
    int *y;
} _asteroid;

asteroid
generate_asteroid(int x, int y, ASTEROID_TYPE type) {
    _asteroid *this = malloc(sizeof(_asteroid));
    this->x = malloc(sizeof(int) * MAX_VERTICES);
    this->y = malloc(sizeof(int) * MAX_VERTICES);

    int min_radius = 30;
    int max_radius = 35;

    this->anchor_x = x;
    this->anchor_y = y;
    this->vertices = (int) random_in_range(7, MAX_VERTICES);
    int radius = (int) floor(random_double() * (max_radius - min_radius) + min_radius);
    double cr_ang = 0.0;
    double an_diff = TO_RADIANS(360 / this->vertices);
    double rad_jitter = radius / 1.0; // tweak divisor for more irregular shapes
    double an_jitter = an_diff * .9;

    int i;
    for (i = 0; i < this->vertices; i++) {
        double t_radius = radius + (random_double() * rad_jitter - rad_jitter / 2.0);
        double t_ang = cr_ang + (random_double() * an_jitter - an_jitter / 2.0);
        int nx = (int) (sin(t_ang) * t_radius);
        int ny = (int) (cos(t_ang) * t_radius);
        this->x[i] = nx + (int) this->anchor_x;
        this->y[i] = ny + (int) this->anchor_y;
        cr_ang += an_diff;
    }
    return this;
}

void
update_asteroid(asteroid a) {
    //TODO implement
}

void
render_asteroid(SDL_Renderer *renderer, asteroid a) {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    _asteroid *this = a;
    int i;
    for (i = 0; i < this->vertices; i++) {
        int x = this->x[i];
        int y = this->y[i];
        int next_x = this->x[(i + 1) % this->vertices];
        int next_y = this->y[(i + 1) % this->vertices];
        SDL_RenderDrawLine(renderer, x, y, next_x, next_y);
    }
}

void
free_asteroid(asteroid a) {
    if (a) {
        _asteroid *this = a;
        free(this->x);
        free(this->y);
        free(this);
    }
}