//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <colors.h>
#include "ship.h"

global const int SHIP_POINTS = 5;
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
} _ship;

typedef struct _bullet {
    int x;
    int y;
} _bullet;

ship
allocate_ship(int x, int y) {
    _ship *s = malloc(sizeof(_ship));
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
        free(s);
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

void
update_ship(ship s, keyboard keys, screen scrn) {

    _ship *this = s;

    if (keys.right_down) {
        this->dir -= 0.1;
        if (this->dir < 0.0) {
            this->dir = RADIANS;
        }
    }

    if (keys.left_down) {
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
    this->y += this->y_vector * .5;

    this->x_delta *= .98;
    this->y_delta *= .98;

    keep_in_bounds(this, scrn.width, scrn.height);
}

void
render_ship(SDL_Renderer *renderer, ship s) {
    _ship *this = s;
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    //TODO eventually lets move this math stuff to update to avoid needless calls to cos/sin
    point points[SHIP_POINTS];

    point p0 = {this->x - 7, this->y + 12};
    points[0] = p0;
    point p1 = {this->x, this->y - 10};
    points[1] = p1;
    point p2 = {this->x + 7, this->y + 12};
    points[2] = p2;
    point p3 = {this->x + 5, this->y + 8};
    points[3] = p3;
    point p4 = {this->x - 5, this->y + 8};
    points[4] = p4;

    int centroid_x = (p0.x + p1.x + p2.x) / 3;
    int centroid_y = (p0.y + p1.y + p2.y) / 3;
    double cosine = cos(this->dir);
    double sine = sin(this->dir);

    int i;
    for (i = 0; i < SHIP_POINTS; i++) {
        int distance_x = points[i].x - centroid_x;
        int distance_y = points[i].y - centroid_y;
        points[i].x = (int) (cosine * distance_x - sine * distance_y + centroid_x);
        points[i].y = (int) (sine * distance_x + cosine * distance_y + centroid_y);
    }

    for (i = 0; i < SHIP_POINTS; i++) {
        SDL_RenderDrawLine(renderer, points[i].x, points[i].y, points[(i + 1) % SHIP_POINTS].x, points[(i + 1) % SHIP_POINTS].y);
    }
}