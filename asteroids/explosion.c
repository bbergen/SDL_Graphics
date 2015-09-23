//
// Created by bryan on 8/27/15.
//

#include <vector.h>
#include <random.h>
#include <common.h>
#include <colors.h>
#include "explosion.h"

#define MAX_DENISTY 25
#define MIN_DENSITY 5
//TODO change ttl range to coefficient on screen size
#define MAX_TTL 30
#define MIN_TTL 20

typedef struct mote {
    double x;
    double y;
    int speed;
    double dir;
} mote;

typedef struct _explosion {
    int x;
    int y;
    vector motes;
    int ttl;
} _explosion;

internal void
update_mote(mote *m) {
    double x_vector = sin(m->dir);
    double y_vector = cos(m->dir);

    m->x += x_vector * m->speed;
    m->y -= y_vector * m->speed;
}

void
update_explosion(explosion e) {
    _explosion *this = e;
    if (this->ttl <= 0) {
        return;
    }
    int i;
    for (i = 0; i < vector_size(this->motes); i++) {
        update_mote(vector_get(this->motes, i));
    }
    this->ttl--;
}

internal void
render_mote(SDL_Renderer *renderer, mote *m) {
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderDrawPoint(renderer, (int) m->x, (int) m->y);
}

void
render_explosion(SDL_Renderer *renderer, explosion e) {
    _explosion *this = e;
    if (this->ttl <= 0) {
        return;
    }
    int i;
    for (i = 0; i < vector_size(this->motes); i++) {
        render_mote(renderer, vector_get(this->motes, i));
    }
}

internal mote*
generate_mote(int x, int y) {
    mote *m = malloc(sizeof(mote));
    m->x = x;
    m->y = y;
    m->dir = random_double() * RADIANS;
    m->speed = (int) random_in_range(2,7);
    return m;
}

internal void
free_mote(mote *m) {
    if (m) {
        free(m);
    }
}


explosion
generate_explosion(int x, int y, screen scrn) {
    _explosion *e = malloc(sizeof(_explosion));
    e->x = x;
    e->y = y;
    int density = (int) random_in_range(MIN_DENSITY, MAX_DENISTY);
    e->motes = vector_init(density);
    int i;
    for (i = 0; i < density; i++) {
        vector_add(e->motes, generate_mote(x, y));
    }
    e->ttl = (int) random_in_range(MIN_TTL, MAX_TTL);
    return e;
}

void
free_explosion(explosion e) {
    if (e) {
        _explosion *this = e;
        int i;
        for (i = 0; i < vector_size(this->motes); i++) {
            free_mote(vector_get(this->motes, i));
        }
        vector_free(this->motes);
        free(this);
    }
}