//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_BREAKER_H
#define SDL_GRAPHICS_BREAKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 576
#define SCREEN_HEIGHT 850
#define AUDIO_FREQUENCY 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNK_SIZE 2048
#define BALL_SIZE 5
#define DEFAULT_PADDLE_SPEED 300
#define STARTING_SPEED 300
#define PADDLE_HEIGHT 20
#define PADDLE_WIDTH 100

#define WALL_PING "resources/sounds/breaker/effects/wall_ping.wav"
#define LEVEL_1_TRACK "resources/sounds/breaker/music/level_1.wav"
#define LEVEL_2_TRACK "resources/sounds/breaker/music/level_2.wav"
#define LEVEL_3_TRACK "resources/sounds/breaker/music/level_3.wav"
#define LEVEL_4_TRACK "resources/sounds/breaker/music/level_4.wav"

typedef struct paddle_end {
    int center_offset;
    int radius;
} paddle_end;

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
    int point_value;
    SDL_Color *color;
} breaker_brick;

typedef struct breaker_paddle {
    float x;
    float y;
    int width;
    int height;
    paddle_end *left_end;
    paddle_end *right_end;
    SDL_Color *color;
} breaker_paddle;

typedef struct breaker_ball {
    float x;
    float y;
    int radius;
    int8_t x_dir;
    int8_t y_dir;
    const SDL_Color *color;
} breaker_ball;

typedef struct breaker_game {
    breaker_ball *ball;
    breaker_paddle *player;
    score_box *score;
    breaker_sounds *sounds;
    int remaining_balls;
    float difficulty_modifier;
} breaker_game;

static const SDL_Color BACKGROUND = {
        0xFF, 0xFF, 0xFF, 0x00
};

static const SDL_Color DEFAULT_BALL = {
        0x4B, 0x00, 0x82, 0x00
};

#endif //SDL_GRAPHICS_BREAKER_H
