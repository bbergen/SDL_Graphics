//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_BREAKER_H
#define SDL_GRAPHICS_BREAKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct breaker_sounds {
    Mix_Music *music;
    Mix_Chunk *paddle_bounce;
    Mix_Chunk *brick_break;
};

typedef struct breaker_brick {
    int x;
    int y;
    int width;
    int height;
    int point_value;
    SDL_Color *color;
} breaker_brick;

typedef struct breaker_paddle {
    float x;
    float y;
    int width;
    int height;
    SDL_Color *color;
} breaker_paddle;

typedef struct breaker_ball {
    float x;
    float y;
    int radius;
    SDL_Color *color;
} breaker_ball;

typedef struct breaker_game {
    breaker_paddle *player;
    breaker_ball *ball;
    float difficulty_modifier;
    uint32_t score;
} breaker_game;

#endif //SDL_GRAPHICS_BREAKER_H
