//
// Created by bryan on 7/21/15.
//
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define BALL_SIZE 15
#define BASE_SPEED 250
#define true 1
#define false 0

typedef struct Ball {
    float x;
    float y;
    int radius;
} Ball;

typedef struct Paddle {
    float x;
    float y;
    float width;
    float height;
} Paddle;

typedef struct Game {
    Ball ball;
    Paddle player;
    Paddle ai;
    int player_score;
    int ai_score;
} Game;

static const SDL_Color BACKGROUND = {
        0x00, 0x00, 0x00, 0x00
};

static const SDL_Color BALL_COLOR = {
        0x00, 0xff, 0x64, 0x00
};

static const SDL_Color PADDLE_COLOR = {
        0x00, 0xFF, 0x64, 0x00
};

static void
update(Game *game) {
    //TODO implement
}

static void
render_ball(SDL_Renderer *renderer, Ball *ball) {
    //TODO implement
}

static void
render_paddle(SDL_Renderer *renderer, Paddle *paddle) {
    //TODO implement
}

static void
render(SDL_Renderer *renderer, Game *game) {
    //TODO implement
}

static void
process_event(SDL_Event *event, Game *game, uint8_t *running) {
    //TODO implement
}

int
main(int argc, char **argv) {
    printf("SDL Pong\n");
    return EXIT_SUCCESS;
}
