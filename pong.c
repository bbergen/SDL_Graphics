//
// Created by bryan on 7/21/15.
//
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define PADDLE_WIDTH 50
#define PADDLE_HEIGHT 100
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
update(Game *game, float delta_t) {
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
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        default:
            break;
    }
}

static void
error(void) {
    fprintf(stderr, "Error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
}

static void
run(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error();
    }

    SDL_Window *window = SDL_CreateWindow("SDL Pong!",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        error();
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error();
    }

    Ball ball = {
            SCREEN_HEIGHT >> 1,
            SCREEN_WIDTH >> 1,
            BALL_SIZE
    };

    Paddle player_paddle = {
            0,
            0,
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };

    Paddle ai_paddle = {
            0,
            0,
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };

    Game game = {
            ball,
            player_paddle,
            ai_paddle,
            0, 0
    };

    SDL_Event event;
    uint32_t then;
    uint32_t now = SDL_GetTicks();
    float delta_t;

    uint8_t running = true;

    // game loop
    while (running) {
        then = now;
        now = SDL_GetTicks();
        delta_t = (now - then) / 1000.0f;

        while (SDL_PollEvent(&event) != 0) {
            process_event(&event, &game, &running);
        }

        update(&game, delta_t);
        render(renderer, &game);
    }
}


int
main(int argc, char **argv) {
    run();
    SDL_Quit();
    return EXIT_SUCCESS;
}
