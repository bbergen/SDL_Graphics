//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <random.h>
#include <colors.h>
#include "asteroid.h"

#define MIN_RADIUS_LARGE .03
#define MIN_RADIUS_SMALL .005
#define MAX_RADIUS_LARGE .05
#define MAX_RADIUS_SMALL .025
#define MIN_ROTATION 0.005
#define MAX_ROTATION 0.03
#define MAX_VERTICES 12
#define MAX_ASTEROID_SPEED 7
#define MIN_ASTEROID_SPEED 2
#define RADIANS (PI * 2)

typedef struct _asteroid {
    ASTEROID_TYPE type;
    int8_t visible;
    double anchor_x;
    double anchor_y;
    double dir;
    double rotation_dir;
    double rotational_speed;
    int radius;
    int speed;
    int vertices;
    int *x_offsets;
    int *y_offsets;
} _asteroid;

asteroid
generate_asteroid(int x, int y, screen scrn, ASTEROID_TYPE type) {
    _asteroid *this = malloc(sizeof(_asteroid));
    this->x_offsets = malloc(sizeof(int) * MAX_VERTICES);
    this->y_offsets = malloc(sizeof(int) * MAX_VERTICES);
    this->type = type;
    this->visible = true;

    double min_radius = this->type == LARGE ? scrn.width * MIN_RADIUS_LARGE : scrn.width * MIN_RADIUS_SMALL;
    double max_radius = this->type == LARGE ? scrn.width * MAX_RADIUS_LARGE : scrn.width * MAX_RADIUS_SMALL;

    // generate random asteroid shape
    this->anchor_x = x;
    this->anchor_y = y;
    this->vertices = (int) random_in_range(7, MAX_VERTICES);
    this->radius = (int) floor(random_double() * (max_radius - min_radius) + min_radius);
    double cr_ang = 0.0;
    double an_diff = TO_RADIANS(360 / this->vertices);
    double rad_jitter = this->radius / 1.0; // tweak divisor for more irregular shapes
    double an_jitter = an_diff * .9;

    int i;
    for (i = 0; i < this->vertices; i++) {
        double t_radius = this->radius + (random_double() * rad_jitter - rad_jitter / 2.0);
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

    if (*x < -MAX_RADIUS_LARGE) {
        *x = scrn.width;
    }

    if (*x > scrn.width + MAX_RADIUS_LARGE) {
        *x = 0;
    }

    if (*y < -MAX_RADIUS_LARGE) {
        *y = scrn.height;
    }

    if (*y > scrn.height + MAX_RADIUS_LARGE) {
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
    _asteroid *this = a;

    if (!this->visible) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

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

int8_t
lines_intersect(point v1_start, point v1_end, point v2_start, point v2_end) {
    int d1, d2;
    int a1, a2, b1, b2, c1, c2;

    a1 = v1_end.y - v1_start.y;
    b1 = v1_start.x - v1_end.x;
    c1 = (v1_end.x * v1_start.y) - (v1_start.x * v1_end.y);

    d1 = (a1 * v2_start.x) + (b1 * v2_start.y) + c1;
    d2 = (a1 * v2_end.x) + (b1 * v2_end.y) + c1;

    if (d1 > 0 && d2 > 0) {
        return false;
    }
    if (d1 < 0 && d2 < 0) {
        return false;
    }

    a2 = v2_end.y - v2_start.y;
    b2 = v2_start.x - v2_end.x;
    c2 = (v2_end.x * v2_start.y) - (v2_start.x * v2_end.y);

    d1 = (a2 * v1_start.x) + (b2 * v1_start.y) +c2;
    d2 = (a2 * v1_end.x) + (b2 * v2_end.y) + c2;

    if (d1 > 0 && d2 > 0) {
        return false;
    }
    if (d1 < 0 && d2 < 0) {
        return false;
    }

    //could be collinear, in which case we will say no
    if ((a1 * b2) - (a2 * b1) == 0) {
        return false;
    }

    return true;
}

int8_t
asteroid_contains(asteroid a, point p) {
    _asteroid *this = a;

    // first we do a speed check to see if its certainly no in the asteroid
    double min_x = this->anchor_x - this->radius;
    double max_x = this->anchor_x + this->radius;
    double min_y = this->anchor_y - this->radius;
    double max_y = this->anchor_y + this->radius;

    if (p.x < min_x || p.x > max_x || p.y < min_y || p.y > max_y) {
        return false;
    }

    // if we haven't determined its outside yet, we need to take a closer look
    double cosine = cos(this->rotation_dir);
    double sine = sin(this->rotation_dir);

    int intersections = 0;
    point p2 = {p.x, (int) max_y + 5};

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

        point v1_start = {rotated_x, rotated_y};
        point v1_end = {next_rotated_x, next_rotated_y};

        point v2_start = p2;
        point v2_end = p;

        if (lines_intersect(v1_start, v1_end, v2_start, v2_end)) {
            intersections++;
        }
    }

    return (intersections & 1) == 1;
}