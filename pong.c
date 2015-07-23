//
// Created by bryan on 7/21/15.
//
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint-gcc.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 576
#define PADDLE_WIDTH 25
#define PADDLE_HEIGHT 75
#define BALL_SIZE 10
#define BALL_SPEED 250
#define PADDLE_SPEED 250
#define VARIANCE 0.025f
#define SCORE_WIDTH 100
#define SCORE_HEIGHT 50
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

static void
update_ball(pong_ball *ball, float delta_t) {
    static int dir_x = 1;
    static int dir_y = 1;
    float speed_x = delta_t * BALL_SPEED;
    float speed_y = delta_t * BALL_SPEED + VARIANCE;

    // check collisions, scores

    // left
    if (ball->x <= ball->radius) {
        ball->x = ball->radius;
        dir_x *= -1;
        //TODO change to ai scores
    }

    // right
    if (ball->x >= SCREEN_WIDTH - ball->radius) {
        ball->x = SCREEN_WIDTH - ball->radius;
        dir_x *= -1;
        //TODO change to player scores
    }

    // top
    if (ball->y <= ball->radius) {
        ball->y = ball->radius;
        dir_y *= -1;
    }

    // bottom
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        ball->y = SCREEN_HEIGHT - ball->radius;
        dir_y *= -1;
    }

    //update position
    ball->x += speed_x * dir_x;
    ball->y += speed_y * dir_y;
}

static void
update_paddle(game_state *game, float delta_t) {
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
update_ai(game_state *game, float delta_t) {

    float paddle_top = game->ai->y;
    float paddle_bottom = paddle_top + PADDLE_HEIGHT;
    float paddle_mid = paddle_bottom + (PADDLE_HEIGHT / 2);
    float ball_mid = game->ball->y;

    if (paddle_mid > ball_mid && paddle_top > 0) {
        game->ai->y -= PADDLE_SPEED * delta_t;
    }

    if (paddle_mid < ball_mid && paddle_bottom < SCREEN_HEIGHT) {
        game->ai->y += PADDLE_SPEED * delta_t;
    }
}

static void
update(game_state *game, float delta_t) {
    update_paddle(game, delta_t);
    update_ai(game, delta_t);
    update_ball(game->ball, delta_t);
}

static void
render_ball(SDL_Renderer *renderer, pong_ball *ball) {
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
render_paddle(SDL_Renderer *renderer, pong_paddle *paddle) {
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
render_score(SDL_Renderer *renderer, score_box *score) {
    //TODO change to render a single score_box, currently just test code

    SDL_Surface *score_surface = TTF_RenderText_Solid(score->font, "Score: ", BALL_COLOR);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, score_surface);
    SDL_RenderCopy(renderer, texture, NULL, score->box);
    SDL_DestroyTexture(texture);
}

static void
render_center_line(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    for (int i = 0; i < SCREEN_HEIGHT; i += 5) {
        SDL_RenderDrawPoint(renderer, SCREEN_WIDTH >> 1, i);
    }
}

static void
render(SDL_Renderer *renderer, game_state *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer,
                           BACKGROUND.r,
                           BACKGROUND.g,
                           BACKGROUND.b,
                           BACKGROUND.a);
    SDL_RenderClear(renderer);

    // draw game objects
    render_center_line(renderer);
    render_paddle(renderer, game->player);
    render_paddle(renderer, game->ai);
    render_score(renderer, game->player_score);
    render_score(renderer, game->ai_score);
    render_ball(renderer, game->ball);

    // update screen
    SDL_RenderPresent(renderer);
}

static void
process_event(SDL_Event *event, game_state *game, uint8_t *running) {
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
error(const char*(*error_function)(void)) {
    fprintf(stderr, "Error: %s\n", error_function());
    exit(EXIT_FAILURE);
}

static void
run(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(SDL_GetError);
    }

    if (TTF_Init() < 0) {
        error(SDL_GetError);
    }

    SDL_Window *window = SDL_CreateWindow("SDL Pong!",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        error(SDL_GetError);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error(SDL_GetError);
    }

    TTF_Font *font;
    font = TTF_OpenFont("resources/fonts/game_over.ttf", 24);
    if (!font) {
        error(TTF_GetError);
    }

    pong_ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE
    };


    pong_paddle player_paddle = {
            SCREEN_WIDTH * .1,
            (SCREEN_HEIGHT >> 1) - (PADDLE_HEIGHT >> 1),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };
    pong_paddle ai_paddle = {
            SCREEN_WIDTH - (SCREEN_WIDTH * .1 + PADDLE_WIDTH),
            (SCREEN_HEIGHT >> 1) - (PADDLE_HEIGHT >> 1),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };

    SDL_Rect score_rect = {
            SCREEN_WIDTH >> 1 - SCORE_WIDTH,
            0,
            SCORE_WIDTH,
            SCORE_HEIGHT
    };

    score_box player = {
            &score_rect,
            font,
            0
    };

    score_box ai = {
            &score_rect,
            font,
            0
    };

    game_state game = {
            &ball,
            &player_paddle,
            &ai_paddle,
            0, 0,
            &player,
            &ai
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
    TTF_CloseFont(font);
}


int
main(int argc, char **argv) {
    run();
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}
