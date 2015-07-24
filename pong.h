//
// Created by bryan on 7/23/15.
//

#ifndef SDL_GRAPHICS_PONG_H
#define SDL_GRAPHICS_PONG_H

#define TEXT_OFFSET 25
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define PADDLE_WIDTH 25
#define PADDLE_HEIGHT 75
#define BALL_SIZE 10
#define BALL_SPEED 250
#define PADDLE_SPEED 250
#define VARIANCE 0.025f
#define SCORE_WIDTH 50
#define SCORE_HEIGHT 75
#define true 1
#define false 0

typedef struct pong_ball {
    float x;
    float y;
    int radius;
} pong_ball;

typedef struct pong_paddle {
    float x;
    float y;
    float width;
    float height;
} pong_paddle;

typedef struct score_box {
    SDL_Rect *box;
    TTF_Font *font;
    int score;
} score_box;

typedef struct game_state {
    pong_ball *ball;
    pong_paddle *player;
    pong_paddle *ai;
    uint8_t key_up_down;
    uint8_t key_down_down;
    score_box *player_score;
    score_box *ai_score;
} game_state;

static const SDL_Color BACKGROUND = {
        0x00, 0x00, 0x00, 0x00
};

static const SDL_Color BALL_COLOR = {
        0x39, 0xFF, 0x14, 0x00
};

static const SDL_Color PADDLE_COLOR = {
        0x39, 0xFF, 0x14, 0x00
};

#endif //SDL_GRAPHICS_PONG_H
