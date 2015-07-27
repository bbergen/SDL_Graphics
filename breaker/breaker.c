//
// Created by bryan on 7/24/15.
//
#include "breaker.h"
#include "common.h"

static void
start_music(Mix_Music *track) {
    Mix_PlayMusic(track, -1);
}

static void
play_sound_effect(Mix_Chunk *effect) {
    Mix_PlayChannel(-1, effect, 0);
}

static void
pause_music(void) {
    if (Mix_PlayingMusic()) {
        if (Mix_PausedMusic()) {
            Mix_ResumeMusic();
        } else {
            Mix_PauseMusic();
        }
    }
}

static void
render_brick(SDL_Renderer *renderer, breaker_brick *brick) {
    //TODO implement
}

static void
render_circle(SDL_Renderer *renderer, const SDL_Color *color, int mid_x, int mid_y, int radius) {
    int x = radius;
    int y = 0;
    int decision_over_2 = 1 - x;

    SDL_SetRenderDrawColor(renderer,
                           color->r,
                           color->g,
                           color->b,
                           color->a);

    while (x >= y) {
        SDL_RenderDrawLine(renderer,
                           x + mid_x,
                           y + mid_y,
                           -x + mid_x,
                           y + mid_y);

        SDL_RenderDrawLine(renderer,
                           -x + mid_x,
                           -y + mid_y,
                           x + mid_x,
                           -y + mid_y);

        SDL_RenderDrawLine(renderer,
                           -y + mid_x,
                           x + mid_y,
                           -y + mid_x,
                           -x + mid_y);

        SDL_RenderDrawLine(renderer,
                           y + mid_x,
                           x + mid_y,
                           y + mid_x,
                           -x + mid_y);

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
render_ball(SDL_Renderer *renderer, breaker_ball *ball) {
    render_circle(renderer, &PURPLE, (int) ball->x, (int) ball->y, ball->radius);
}

static void
render_paddle(SDL_Renderer *renderer, breaker_paddle *paddle) {
    SDL_Rect rect = {
            (int) paddle->x, (int) paddle->y, paddle->width, paddle->height
    };
    SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, &rect);
}

static void
render(SDL_Renderer *renderer, breaker_game *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer,
                           WHITE.r,
                           WHITE.g,
                           WHITE.b,
                           WHITE.a);
    SDL_RenderClear(renderer);

    // render game objects
    render_ball(renderer, game->ball);
    render_paddle(renderer, game->player);

    // present backbuffer
    SDL_RenderPresent(renderer);
}

static void
update_ball(breaker_game *game, float delta_t) {
    float speed_x = delta_t * STARTING_SPEED;
    float speed_y = delta_t * STARTING_SPEED;
    breaker_ball *ball = game->ball;

    // check left
    if (ball->x <= ball->radius) {
        ball->x = ball->radius;
        ball->x_dir *= -1;
        play_sound_effect(game->sounds->wall_bounce);
    }

    // check right
    if (ball->x >= SCREEN_WIDTH - ball->radius) {
        ball->x = SCREEN_WIDTH - ball->radius;
        ball->x_dir *= -1;
        play_sound_effect(game->sounds->wall_bounce);
    }

    // check top
    if (ball->y <= ball->radius) {
        ball->y = ball->radius;
        ball->y_dir *= -1;
        play_sound_effect(game->sounds->wall_bounce);
    }

    // check bottom
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        ball->y = SCREEN_HEIGHT - ball->radius;
        ball->y_dir *= -1;
        play_sound_effect(game->sounds->wall_bounce);
    }

    // check paddle collision
    breaker_paddle *paddle = game->player;
    if (ball->x - ball->radius <= paddle->x + PADDLE_WIDTH &&
            ball->x + ball->radius >= paddle->x &&
            ball->y + ball->radius >= paddle->y &&
            ball->y - ball->radius <= paddle->y + PADDLE_HEIGHT) {
        ball->y_dir *= -1;
        play_sound_effect(game->sounds->wall_bounce);
    }


    // update position
    ball->x += speed_x * ball->x_dir;
    ball->y += speed_y * ball->y_dir;
}

static void
update_paddle(breaker_paddle *paddle, int8_t right_down, int8_t left_down, float delta_t) {

    if (right_down) {
        if (paddle->x <= SCREEN_WIDTH - PADDLE_WIDTH) {
            paddle->x += STARTING_SPEED * delta_t;
        } else {
            paddle->x = SCREEN_WIDTH - PADDLE_WIDTH;
        }
    }

    if (left_down) {
        if (paddle->x >= 0) {
            paddle->x -= STARTING_SPEED * delta_t;
        } else {
            paddle->x = 0;
        }
    }
}

static void
update(breaker_game *game, float delta_t) {
    update_ball(game, delta_t);
    update_paddle(game->player, game->key_right_down, game->key_left_down, delta_t);
}

static void
process_event(SDL_Event *event, breaker_game * game, int8_t *running) {
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
                case SDLK_RIGHT:
                    game->key_right_down = true;
                    break;
                case SDLK_LEFT:
                    game->key_left_down = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_RIGHT:
                    game->key_right_down = false;
                    break;
                case SDLK_LEFT:
                    game->key_left_down = false;
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
    SDL_Window *window = SDL_CreateWindow("Brick Breaker!",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        error(SDL_GetError);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, - 1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error(SDL_GetError);
    }

    //TODO load fonts
    //TODO load music/audio chunks

    Mix_Music *level_1 = Mix_LoadMUS(LEVEL_1_TRACK);

    if (!level_1) {
        error(Mix_GetError);
    }

    Mix_Chunk *wall_ping = Mix_LoadWAV(WALL_PING);

    if (!wall_ping) {
        error(Mix_GetError);
    }

    breaker_sounds sounds = {
            level_1,
            wall_ping
    };

    breaker_ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE,
            1, 1,
    };

    breaker_paddle paddle = {
            (SCREEN_WIDTH >> 1) - (PADDLE_WIDTH >> 1),
            SCREEN_HEIGHT * .9f,
            PADDLE_WIDTH,
            PADDLE_HEIGHT,
    };

    breaker_game game = {
            &ball,
            &paddle,
            NULL,
            &sounds,
            false,
            false
    };

    int8_t running = true;
    SDL_Event event;
    uint32_t then;
    uint32_t now = SDL_GetTicks();
    float delta_t;

    //start music
    Mix_PlayMusic(game.sounds->music, -1);

    //game loop
    while (running) {
        then = now;
        now = SDL_GetTicks();
        delta_t = (now - then) / 1000.0f;

        while (SDL_PollEvent(&event) !=0) {
            process_event(&event, &game, &running);
        }

        update(&game, delta_t);
        render(renderer, &game);
    }

    //free resources
    Mix_FreeMusic(level_1);
}

static void
init(void) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        error(SDL_GetError);
    }

    if (TTF_Init() < 0) {
        error(TTF_GetError);
    }

    if (Mix_OpenAudio(AUDIO_FREQUENCY,
                      MIX_DEFAULT_FORMAT,
                      AUDIO_CHANNELS,
                      AUDIO_CHUNK_SIZE) < 0) {
        error(Mix_GetError);
    }

}

static void
close(void) {
    SDL_Quit();
}

int
main(int argc, char** argv) {
    init();
    run();
    close();
    return EXIT_SUCCESS;
}
