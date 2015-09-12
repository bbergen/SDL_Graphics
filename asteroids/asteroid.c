//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <random.h>
#include <colors.h>
#include "asteroid.h"

#define MIN_ROTATION 0.005
#define MAX_ROTATION 0.03
#define MAX_VERTICES 12
#define MAX_ASTEROID_SPEED 7
#define MIN_ASTEROID_SPEED 2
#define RADIANS (PI * 2)

typedef struct _asteroid {
    ASTEROID_TYPE type;
    double anchor_x;
    double anchor_y;
    double dir;
    double rotation_dir;
    double rotational_speed;
    int speed;
    int vertices;
    int *x_offsets;
    int *y_offsets;
} _asteroid;

asteroid
generate_asteroid(int x, int y, ASTEROID_TYPE type) {
    _asteroid *this = malloc(sizeof(_asteroid));
    this->x_offsets = malloc(sizeof(int) * MAX_VERTICES);
    this->y_offsets = malloc(sizeof(int) * MAX_VERTICES);
    this->type = type;

    int min_radius = 50;
    int max_radius = 40;

    // generate random asteroid shape
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
        this->x_offsets[i] = nx;
        this->y_offsets[i] = ny;
        cr_ang += an_diff;
    }

    // generate random asteroid vector
    this->dir = random_double() * RADIANS;

    // generate random asteroid speed
    this->speed = (int) random_in_range(MIN_ASTEROID_SPEED, MAX_ASTEROID_SPEED);

    // generate random asteroid rotation
    this->rotation_dir = RADIANS;
    double rotation_speed = random_double() * (MAX_ROTATION - MIN_ROTATION) + MAX_ROTATION;
    rotation_speed *= random_bool() ? 1 : -1;
    this->rotational_speed = rotation_speed;

    return this;
}

internal void
enforce_bounds(double *x, double *y, screen scrn) {

    if (*x < 0) {
        *x = scrn.width;
    }

    if (*x > scrn.width) {
        *x = 0;
    }

    if (*y < 0) {
        *y = scrn.height;
    }

    if (*y > scrn.height) {
        *y = 0;
    }
}

void
update_asteroid(asteroid a, screen scrn) {
    _asteroid *this = a;
    double x_vector = sin(this->dir);
    double y_vector = cos(this->dir);
    this->anchor_x += x_vector * this->speed;
    this->anchor_y -= y_vector * this->speed;

    this->rotation_dir += this->rotational_speed;
    if (this->rotation_dir > RADIANS) {
        this->rotation_dir = 0.0;
    } else if (this->rotation_dir < 0.0 ) {
        this->rotation_dir = RADIANS;
    }

    enforce_bounds(&this->anchor_x, &this->anchor_y, scrn);
}

void
render_asteroid(SDL_Renderer *renderer, asteroid a) {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    _asteroid *this = a;

    double cosine = cos(this->rotation_dir);
    double sine = sin(this->rotation_dir);

    int i;
    for (i = 0; i < this->vertices; i++) {

        int distance_x = -this->x_offsets[i];
        int distance_y = -this->y_offsets[i];

        int rotated_x = (int) (cosine * distance_x - sine * distance_y + this->anchor_x);
        int rotated_y = (int) (sine * distance_x + cosine * distance_y + this->anchor_y);

        int d_next_x = -this->x_offsets[(i + 1) % this->vertices];
        int d_next_y = -this->y_offsets[(i + 1) % this->vertices];

        int next_rotated_x = (int) (cosine * d_next_x - sine * d_next_y + this->anchor_x);
        int next_rotated_y = (int) (sine * d_next_x + cosine * d_next_y + this->anchor_y);

        SDL_RenderDrawLine(renderer, rotated_x, rotated_y, next_rotated_x, next_rotated_y);
    }
}

void
free_asteroid(asteroid a) {
    if (a) {
        _asteroid *this = a;
        free(this->x_offsets);
        free(this->y_offsets);
        free(this);
    }
}