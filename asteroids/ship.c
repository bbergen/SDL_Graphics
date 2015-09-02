//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <colors.h>
#include "ship.h"

#define SHIP_POINTS 5

global const double RADIANS = PI * 2;
global const double ACCELERATION_FACTOR = 0.25;

typedef struct _ship {
    int x;
    int y;
    double x_vector;
    double y_vector;
    double x_delta;
    double y_delta;
    double dir;
    point *vertices;
} _ship;

typedef struct _bullet {
    int x;
    int y;
} _bullet;

ship
allocate_ship(int x, int y) {
    _ship *s = malloc(sizeof(_ship));
    s->vertices = malloc(sizeof(point) * SHIP_POINTS);
    s->x = x;
    s->y = y;
    s->x_vector = 0.0;
    s->y_vector = 0.0;
    s->x_delta = 0.0;
    s->y_delta = 0.0;
    s->dir = RADIANS;
    return s;
}

void
free_ship(ship s) {
    if (s) {
        _ship *this = s;
        free(this->vertices);
        free(this);
    }
}

void
keep_in_bounds(_ship *this, int width, int height) {
    if (this->x > width) {
        this->x = 0;
    }
    if (this->x < 0) {
        this->x = width;
    }
    if (this->y > height) {
        this->y = 0;
    }
    if (this->y < 0) {
        this->y = height;
    }
}

internal void
update_ship_impl(_ship *this, screen scrn) {

    point p0 = {this->x - (scrn.height / 109), this->y + (scrn.height / 64)};
    this->vertices[0] = p0;
    point p1 = {this->x, this->y - (scrn.height / 77)};
    this->vertices[1] = p1;
    point p2 = {this->x + (scrn.height / 109), this->y + (scrn.height / 64)};
    this->vertices[2] = p2;
    point p3 = {this->x + (scrn.height / 154), this->y + (scrn.height / 96)};
    this->vertices[3] = p3;
    point p4 = {this->x - (scrn.height / 154), this->y + (scrn.height / 96)};
    this->vertices[4] = p4;

    int centroid_x = (p0.x + p1.x + p2.x) / 3;
    int centroid_y = (p0.y + p1.y + p2.y) / 3;
    double cosine = cos(this->dir);
    double sine = sin(this->dir);

    int i;
    for (i = 0; i < SHIP_POINTS; i++) {
        int distance_x = this->vertices[i].x - centroid_x;
        int distance_y = this->vertices[i].y - centroid_y;
        this->vertices[i].x = (int) (cosine * distance_x - sine * distance_y + centroid_x);
        this->vertices[i].y = (int) (sine * distance_x + cosine * distance_y + centroid_y);
    }
}

void
update_ship(ship s, keyboard keys, screen scrn) {

    _ship *this = s;

    if (keys.left_down) {
        this->dir -= 0.1;
        if (this->dir < 0.0) {
            this->dir = RADIANS;
        }
    }

    if (keys.right_down) {
        this->dir += 0.1;
        if (this->dir > RADIANS) {
            this->dir = 0.0;
        }
    }

    if (keys.up_down) {
        this->x_vector = sin(this->dir);
        this->y_vector = cos(this->dir);
        this->x_delta += sin(this->dir) * ACCELERATION_FACTOR;
        this->y_delta += cos(this->dir) * ACCELERATION_FACTOR;
        this->x_vector += this->x_delta;
        this->y_vector += this->y_delta;
    }

    this->x += this->x_vector * .5;
    this->y -= this->y_vector * .5;

    this->x_delta *= .98;
    this->y_delta *= .98;

    keep_in_bounds(this, scrn.width, scrn.height);
    update_ship_impl(this, scrn);
}

void
render_ship(SDL_Renderer *renderer, ship s) {
    _ship *this = s;
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    int i;
    for (i = 0; i <SHIP_POINTS; i++) {
        point p = this->vertices[i];
        point next = this->vertices[(i + 1) % SHIP_POINTS];
        SDL_RenderDrawLine(renderer, p.x, p.y, next.x, next.y);
    }
}