//
// Created by bryan on 8/27/15.
//

#include <common.h>
#include <colors.h>
#include <list.h>
#include "ship.h"

global const int SHIP_POINTS = 5;
global const int ENGINE_POINTS = 3;
global const double RADIANS = PI * 2;
global const double ACCELERATION_FACTOR = 0.25;
global const double BASE_BULLET_DELTA = 5;
global const int BASE_BULLET_TTL = 100;

typedef struct _ship {
    int x;
    int y;
    uint32_t flicker;
    double x_vector;
    double y_vector;
    double x_delta;
    double y_delta;
    double dir;
    point *ship_vertices;
    point *engine_vertices;
    list *bullets;
    int8_t thrusting;
} _ship;

typedef struct _bullet {
    int x;
    int y;
    double x_delta;
    double y_delta;
    int ttl;
} _bullet;


internal void
prune_bullets(list *bullets) {
    if (bullets && bullets->head) {
        _bullet *bullet = bullets->head->data;
        if (bullet && bullet->ttl <= 0) {
            list_remove(bullets, 0);
#if DEBUG
            printf("Pruning Bullet. List Size: %d\n", list_size(bullets));
#endif
        }
    }
}

internal int8_t
update_bullets(void *b, void *scrn) {
    _bullet *bullet = b;
    screen *s = scrn;
    bullet->x += bullet->x_delta;
    bullet->y += bullet->y_delta;

    if (bullet->x < 0) {
        bullet->x = s->width;
    }

    if (bullet->x > s->width) {
        bullet->x = 0;
    }

    if (bullet->y < 0) {
        bullet->y = s->height;
    }

    if (bullet->y > s->height) {
        bullet->y = 0;
    }

    bullet->ttl--;
    return true;
}

internal int8_t
render_bullets(void *b, void *r) {
    _bullet *bullet = b;

    if (bullet->ttl <= 0) {
        return true;
    }

    SDL_Renderer *renderer = r;
    SDL_SetRenderDrawColor(renderer, RED.r, RED.g, RED.b, RED.a);
    SDL_RenderDrawPoint(renderer, bullet->x, bullet->y);
    return true;
}

ship
allocate_ship(int x, int y) {
    _ship *s = malloc(sizeof(_ship));
    s->bullets = malloc(sizeof(list));
    list_init(s->bullets, sizeof(_bullet), NULL);
    s->ship_vertices = malloc(sizeof(point) * SHIP_POINTS);
    s->engine_vertices = malloc(sizeof(point) * ENGINE_POINTS);
    s->x = x;
    s->y = y;
    s->thrusting = false;
    s->flicker = 0;
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
        free(this->ship_vertices);
        free(this->engine_vertices);
        list_free(this->bullets);
        free(this->bullets);
        free(this);
    }
}

internal _bullet
new_bullet(_ship *s, point nose) {
    _bullet bullet = {
            nose.x,
            nose.y,
            s->x_delta + BASE_BULLET_DELTA,
            s->y_delta + BASE_BULLET_DELTA,
            BASE_BULLET_TTL
    };
    return bullet;
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
update_ship_impl(_ship *this, keyboard keys, screen scrn) {

    // ship points
    point p0 = {this->x - (scrn.height / 109), this->y + (scrn.height / 64)};
    this->ship_vertices[0] = p0;
    point p1 = {this->x, this->y - (scrn.height / 77)};
    this->ship_vertices[1] = p1;
    point p2 = {this->x + (scrn.height / 109), this->y + (scrn.height / 64)};
    this->ship_vertices[2] = p2;
    point p3 = {this->x + (scrn.height / 154), this->y + (scrn.height / 96)};
    this->ship_vertices[3] = p3;
    point p4 = {this->x - (scrn.height / 154), this->y + (scrn.height / 96)};
    this->ship_vertices[4] = p4;

    // engine points
    point e0 = {this->x - (scrn.height / 256), this->y + (scrn.height / 96)};
    this->engine_vertices[0] = e0;
    point e1 = {this->x, this->y + scrn.height / 51};
    this->engine_vertices[1] = e1;
    point e2 = {this->x + (scrn.height / 256), this->y + (scrn.height / 96)};
    this->engine_vertices[2] = e2;

    int centroid_x = (p0.x + p1.x + p2.x) / 3;
    int centroid_y = (p0.y + p1.y + p2.y) / 3;
    double cosine = cos(this->dir);
    double sine = sin(this->dir);

    int i;
    for (i = 0; i < SHIP_POINTS; i++) {
        int distance_x = this->ship_vertices[i].x - centroid_x;
        int distance_y = this->ship_vertices[i].y - centroid_y;
        this->ship_vertices[i].x = (int) (cosine * distance_x - sine * distance_y + centroid_x);
        this->ship_vertices[i].y = (int) (sine * distance_x + cosine * distance_y + centroid_y);
    }

    for (i = 0; i < ENGINE_POINTS; i++) {
        int distance_x = this->engine_vertices[i].x - centroid_x;
        int distance_y = this->engine_vertices[i].y - centroid_y;
        this->engine_vertices[i].x = (int) (cosine * distance_x - sine * distance_y + centroid_x);
        this->engine_vertices[i].y = (int) (sine * distance_x + cosine * distance_y + centroid_y);
    }

    if (keys.space_down) {
        _bullet bullet = new_bullet(this, this->ship_vertices[1]);
        list_add(this->bullets, &bullet);
    }
    list_for_each_with_param(this->bullets, update_bullets, &scrn);

    prune_bullets(this->bullets);
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

    this->x_delta *= .995;
    this->y_delta *= .995;

    this->thrusting = keys.up_down;
    this->flicker++;
    keep_in_bounds(this, scrn.width, scrn.height);
    update_ship_impl(this, keys, scrn);
}

void
render_ship(SDL_Renderer *renderer, ship s) {
    _ship *this = s;
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);

    int i;
    for (i = 0; i < SHIP_POINTS; i++) {
        point p = this->ship_vertices[i];
        point next = this->ship_vertices[(i + 1) % SHIP_POINTS];
        SDL_RenderDrawLine(renderer, p.x, p.y, next.x, next.y);
    }

    if (this->thrusting && this->flicker % 5 == 0) {
        SDL_SetRenderDrawColor(renderer, DARK_ORANGE.r, DARK_ORANGE.g, DARK_ORANGE.b, DARK_ORANGE.a);
        for (i = 0; i < ENGINE_POINTS; i++) {
            point p = this->engine_vertices[i];
            point next = this->engine_vertices[(i + 1) % ENGINE_POINTS];
            SDL_RenderDrawLine(renderer, p.x, p.y, next.x, next.y);
        }
    }
    list_for_each_with_param(this->bullets, render_bullets, renderer);
}

int8_t
ship_thrusting(ship s) {
    assert(s);
    return ((_ship*)s)->thrusting;
}