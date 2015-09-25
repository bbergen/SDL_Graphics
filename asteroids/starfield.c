//
// Created by bryan on 9/25/15.
//

#include <vector.h>
#include <colors.h>
#include <random.h>
#include "starfield.h"

#define DENSITY 0.000075

typedef enum STAR_TYPE {
    DWARF, MEDIUM, GIANT
} STAR_TYPE;

typedef struct star {
    int x;
    int y;
    STAR_TYPE type;
    SDL_Color color;
} star;

typedef struct _star_field {
    vector stars;
} _star_field;

internal star*
generate_star(int x, int y) {
    star *s = malloc(sizeof(star));
    s->x = x;
    s->y = y;
    s->type = random_bool() ? DWARF : random_bool() ? GIANT : MEDIUM;
    s->color = random_bool() ? WHITE : random_bool() ? PALE_ORANGE : PALE_YELLOW;
    return s;
}

internal void
free_star(star *s) {
    if (s) {
        free(s);
    }
}

internal void
render_star(SDL_Renderer *renderer, star *s) {
    SDL_SetRenderDrawColor(renderer, s->color.r, s->color.g, s->color.b, s->color.a);
    switch (s->type) {
        case GIANT:
            SDL_RenderDrawLine(renderer, s->x, s->y - 1, s->x, s->y + 1);
            SDL_RenderDrawLine(renderer, s->x - 1, s->y, s->x + 1, s->y);
            break;
        case MEDIUM:
            SDL_RenderDrawPoint(renderer, s->x, s->y);
            SDL_RenderDrawPoint(renderer, s->x, s->y - 1);
            break;
        case DWARF:
        default:
            SDL_RenderDrawPoint(renderer, s->x, s->y);
            break;
    }
}

void
render_star_field(SDL_Renderer *renderer, star_field s) {
    _star_field *sf = s;
    int i;
    for (i = 0; i < vector_size(sf->stars); i++) {
        render_star(renderer, vector_get(sf->stars, i));
    }
}

star_field init_star_field(screen scrn) {
    _star_field *sf = malloc(sizeof(_star_field));
    sf->stars = vector_init(50);
    int i;
    for (i = 0; i < scrn.width; i++) {
        int k;
        for (k = 0; k < scrn.height; k++) {
            if (random_bool_with_probability(DENSITY)) {
                star *s = generate_star(i, k);
                vector_add(sf->stars, s);
            }
        }
    }

    return sf;
}

void free_star_field(star_field s) {
    _star_field *sf = s;
    int i;
    for (i = 0; i < vector_size(sf->stars); i++) {
        free_star(vector_get(sf->stars, i));
    }
    free(sf);
}