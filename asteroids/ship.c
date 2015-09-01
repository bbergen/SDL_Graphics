//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <colors.h>
#include "ship.h"

internal const int SHIP_SPEED = 1;

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
    SDL_RenderDrawPoint(renderer, this->x, this->y);
}