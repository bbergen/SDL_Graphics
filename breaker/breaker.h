//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_BREAKER_H
#define SDL_GRAPHICS_BREAKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "list.h"

#define SCREEN_WIDTH 576
#define SCREEN_HEIGHT 850
#define AUDIO_FREQUENCY 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNK_SIZE 2048
#define BALL_SIZE 5
#define DEFAULT_PADDLE_SPEED 300
#define STARTING_SPEED 300
#define PADDLE_HEIGHT 10
#define PADDLE_WIDTH 75
#define BRICK_WIDTH SCREEN_WIDTH / 12
#define BRICK_HEIGHT 15

#define WALL_PING "resources/sounds/breaker/effects/wall_ping.wav"
#define LEVEL_1_TRACK "resources/sounds/breaker/music/level_1.wav"
#define LEVEL_2_TRACK "resources/sounds/breaker/music/level_2.wav"
#define LEVEL_3_TRACK "resources/sounds/breaker/music/level_3.wav"
#define LEVEL_4_TRACK "resources/sounds/breaker/music/level_4.wav"

typedef struct score_box {
    SDL_Rect *bounds;
    TTF_Font *font;
    uint32_t current_score;
    uint32_t high_score;
} score_box;

typedef struct breaker_sounds {
    Mix_Music *music;
    Mix_Chunk *wall_bounce;
    Mix_Chunk *paddle_bounce;
    Mix_Chunk *brick_break;
} breaker_sounds;

typedef struct breaker_brick {
    int x;
    int y;
    int width;
    int height;
    int8_t visible;
    SDL_Color *color;
    SDL_Renderer *renderer;
} breaker_brick;

typedef struct breaker_paddle {
    float x;
    float y;
    int width;
    int height;
} breaker_paddle;

typedef struct breaker_ball {
    float x;
    float y;
    int radius;
    int8_t x_dir;
    int8_t y_dir;
} breaker_ball;

typedef struct breaker_game {
    breaker_ball *ball;
    breaker_paddle *player;
    score_box *score;
    breaker_sounds *sounds;
    int8_t key_right_down;
    int8_t key_left_down;
    list *brick_list;
} breaker_game;

static const SDL_Color WHITE = {
        0xFF, 0xFF, 0xFF, 0x00
};

static const SDL_Color PURPLE = {
        0x4B, 0x00, 0x82, 0x00
};

static const SDL_Color BLACK = {
        0x00, 0x00, 0x00, 0x00
};

static const SDL_Color BLUE = {
        0x14, 0xAD, 0xDB, 0x00
};

#endif //SDL_GRAPHICS_BREAKER_H
