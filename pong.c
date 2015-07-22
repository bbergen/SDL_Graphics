//
// Created by bryan on 7/21/15.
//
#include <SDL2/SDL.h>
#include <stdint-gcc.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define PADDLE_WIDTH 25
#define PADDLE_HEIGHT 75
#define BALL_SIZE 10
#define BALL_SPEED 250
#define PADDLE_SPEED 250
#define VARIANCE 0.025f
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
    Ball *ball;
    Paddle *player;
    Paddle *ai;
    uint8_t key_up_down;
    uint8_t key_down_down;
    int player_score;
    int ai_score;
} Game;

static const SDL_Color BACKGROUND = {
        0x00, 0x00, 0x00, 0x00
};

static const SDL_Color BALL_COLOR = {
        0x39, 0xFF, 0x14, 0x00
};

static const SDL_Color PADDLE_COLOR = {
        0x39, 0xFF, 0x14, 0x00
};

static void
update_ball(Ball *ball, float delta_t) {
    //TODO implement
}

static void
update_paddle(Game *game, float delta_t) {
    if (game->key_up_down) {
        if (game->player->y > 0) {
            game->player->y -= PADDLE_SPEED * delta_t;
        } else {
            game->player->y = 0;
        }
    }
    if (game->key_down_down) {
        if (game->player->y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            game->player->y += PADDLE_SPEED * delta_t;
        } else {
            game->player->y = SCREEN_HEIGHT - PADDLE_HEIGHT;
        }
    }
}


static void
update_ai(Game *game, float delta_t) {
    //TODO implement
}

static void
update(Game *game, float delta_t) {
    update_paddle(game, delta_t);
    update_ai(game, delta_t);
    update_ball(game->ball, delta_t);
}

static void
render_ball(SDL_Renderer *renderer, Ball *ball) {
    int x = ball->radius;
    int y = 0;
    int decision_over_2 = 1 - x;

    SDL_SetRenderDrawColor(renderer,
                           BALL_COLOR.r,
                           BALL_COLOR.g,
                           BALL_COLOR.b,
                           BALL_COLOR.a);

    while (x >= y) {
        SDL_RenderDrawLine(renderer,
                           x + ball->x,
                           y + ball->y,
                           -x + ball->x,
                           y + ball->y);

        SDL_RenderDrawLine(renderer,
                           -x + ball->x,
                           -y + ball->y,
                           x + ball->x,
                           -y + ball->y);

        SDL_RenderDrawLine(renderer,
                           -y + ball->x,
                           x + ball->y,
                           -y + ball->x,
                           -x + ball->y);

        SDL_RenderDrawLine(renderer,
                           y + ball->x,
                           x + ball->y,
                           y + ball->x,
                           -x + ball->y);

        y++;
        if (decision_over_2 <= 0) {
            decision_over_2 += 2 * y + 1;
        } else {
            x--;
            decision_over_2 += 2 * (y - x) + 1;
        }
    }
}

static void
render_paddle(SDL_Renderer *renderer, Paddle *paddle) {
    SDL_SetRenderDrawColor(renderer,
                           PADDLE_COLOR.r,
                           PADDLE_COLOR.g,
                           PADDLE_COLOR.b,
                           PADDLE_COLOR.a);

    SDL_Rect rect = {
            paddle->x,
            paddle->y,
            paddle->width,
            paddle->height
    };
    SDL_RenderFillRect(renderer, &rect);
}

static void
render_score(SDL_Renderer *renderer, int player, int ai) {

}

static void
render(SDL_Renderer *renderer, Game *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer,
                           BACKGROUND.r,
                           BACKGROUND.g,
                           BACKGROUND.b,
                           BACKGROUND.a);
    SDL_RenderClear(renderer);

    // draw game objects
    render_ball(renderer, game->ball);
    render_paddle(renderer, game->player);
    render_paddle(renderer, game->ai);
    render_score(renderer, game->player_score, game->ai_score);

    // update screen
    SDL_RenderPresent(renderer);
}

static void
process_event(SDL_Event *event, Game *game, uint8_t *running) {
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_UP:
                    game->key_up_down = true;
                    break;
                case SDLK_DOWN:
                    game->key_down_down = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_UP:
                    game->key_up_down = false;
                    break;
                case SDLK_DOWN:
                    game->key_down_down = false;
                    break;
                default:
                    break;
            }
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
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE
    };


    Paddle player_paddle = {
            SCREEN_WIDTH * .1,
            (SCREEN_HEIGHT >> 1) - (PADDLE_HEIGHT >> 1),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };
    Paddle ai_paddle = {
            SCREEN_WIDTH - (SCREEN_WIDTH * .1 + PADDLE_WIDTH),
            (SCREEN_HEIGHT >> 1) - (PADDLE_HEIGHT >> 1),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };

    Game game = {
            &ball,
            &player_paddle,
            &ai_paddle,
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
