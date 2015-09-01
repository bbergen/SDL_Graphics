//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <colors.h>
#include "ship.h"

global const int SHIP_SPEED = 1;
global const int SHIP_POINTS = 5;

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
    s->y = y;
    return s;
}

void
free_ship(ship s) {
    if (s) {
        free(s);
    }
}

void
update_ship(ship s, keyboard keys, screen scrn) {

    _ship *this = s;

    if (keys.down_down) {
        this->y += SHIP_SPEED;
        if (this->y > scrn.height) {
            this->y = 0;
        }
    }

    if (keys.up_down) {
        this->y -= SHIP_SPEED;
        if (this->y < 0) {
            this->y = scrn.height;
        }
    }

    if (keys.left_down) {
        this->x -= SHIP_SPEED;
        if (this->x < 0) {
            this->x = scrn.width;
        }
    }

    if (keys.right_down) {
        this->x += SHIP_SPEED;
        if (this->x > scrn.width) {
            this->x = 0;
        }
    }
}

void
render_ship(SDL_Renderer *renderer, ship s) {
    _ship *this = s;
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    int x_vertices[SHIP_POINTS];
    int y_vertices[SHIP_POINTS];

    x_vertices[0] = this->x - 7;
    x_vertices[1] = this->x;
    x_vertices[2] = this->x + 7;
    x_vertices[3] = this->x + 5;
    x_vertices[4] = this->x - 5;

    y_vertices[0] = this->y + 12;
    y_vertices[1] = this->y - 10;
    y_vertices[2] = this->y + 12;
    y_vertices[3] = this->y + 8;
    y_vertices[4] = this->y + 8;

    int i;
    for (i = 0; i < SHIP_POINTS; i++) {
        SDL_RenderDrawLine(renderer, x_vertices[i], y_vertices[i], x_vertices[(i + 1) % SHIP_POINTS], y_vertices[(i + 1) % SHIP_POINTS]);
    }
}