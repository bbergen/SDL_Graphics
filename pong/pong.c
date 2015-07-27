//
// Created by bryan on 7/21/15.
//
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdint-gcc.h>
#include "pong.h"
#include "common.h"

static int8_t
random_bool(void) {
    return rand() % 2 == 0;
}

static void
play_sound_effect(Mix_Chunk *chunk) {
    Mix_PlayChannel(-1, chunk, 0);
}

static void
reset_game(game_state *game) {
    game->x_dir = random_bool() ? 1 : -1;
    game->y_dir = random_bool() ? 1 : -1;
    game->ball->x = SCREEN_WIDTH >> 1;
    game->ball->y = SCREEN_HEIGHT >> 1;
}

static int8_t
has_collided(pong_ball *ball, pong_paddle *paddle) {
    return ball->x >= paddle->x &&
            ball->x <= paddle->x + PADDLE_WIDTH &&
            ball->y >= paddle->y &&
            ball->y <= paddle->y + PADDLE_HEIGHT;
}

static void
update_ball(game_state *game, float delta_t) {
    float speed_x = delta_t * BALL_SPEED;
    float speed_y = delta_t * BALL_SPEED + VARIANCE;

    pong_ball *ball = game->ball;

    // left, ai scores
    if (ball->x <= ball->radius) {
        game->ai_score->score++;
        play_sound_effect(game->sound->score_sound);
        reset_game(game);
        return;
    }

    // right, player scores
    if (ball->x >= SCREEN_WIDTH - ball->radius) {
        ball->x = SCREEN_WIDTH - ball->radius;
        game->player_score->score++;
        play_sound_effect(game->sound->score_sound);
        reset_game(game);
        return;
    }

    // top
    if (ball->y <= ball->radius) {
        ball->y = ball->radius;
        game->y_dir *= -1;
        play_sound_effect(game->sound->wall_bounce);
    }

    // bottom
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        ball->y = SCREEN_HEIGHT - ball->radius;
        game->y_dir *= -1;
        play_sound_effect(game->sound->wall_bounce);
    }

    // check paddle collisions
    if (has_collided(ball, game->ai)) {
        if (game->x_dir == 1) {
            ball->x = game->ai->x - ball->radius;
        }
        game->x_dir *= -1;
        game->y_dir *= random_bool() ? -1 : 1;
        play_sound_effect(game->sound->paddle_bounce);
    }

    if (has_collided(ball, game->player)) {
        if (game->x_dir == -1) {
            ball->x = game->player->x + PADDLE_WIDTH + ball->radius;
        }
        game->x_dir *= -1;
        game->y_dir *= random_bool() ? -1 : 1;
        play_sound_effect(game->sound->paddle_bounce);
    }

    //update position
    ball->x += speed_x * game->x_dir;
    ball->y += speed_y * game->y_dir;

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
    float paddle_mid = paddle_top + (PADDLE_HEIGHT / 2);
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
    update_ball(game, delta_t);
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
                           x + (int) ball->x,
                           y + (int) ball->y,
                           -x + (int) ball->x,
                           y + (int) ball->y);

        SDL_RenderDrawLine(renderer,
                           -x + (int) ball->x,
                           -y + (int) ball->y,
                           x + (int) ball->x,
                           -y + (int) ball->y);

        SDL_RenderDrawLine(renderer,
                           -y + (int) ball->x,
                           x + (int) ball->y,
                           -y + (int) ball->x,
                           -x + (int) ball->y);

        SDL_RenderDrawLine(renderer,
                           y + (int) ball->x,
                           x + (int) ball->y,
                           y + (int) ball->x,
                           -x + (int) ball->y);

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
            (int) paddle->x,
            (int) paddle->y,
            (int) paddle->width,
            (int) paddle->height
    };
    SDL_RenderFillRect(renderer, &rect);
}

static void
render_score(SDL_Renderer *renderer, score_box *score) {
    char score_string[5];
    sprintf(score_string, score->score < 10 ? "0%d" : "%d", score->score);
    SDL_Surface *score_surface = TTF_RenderText_Solid(score->font, score_string, BALL_COLOR);
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
                           WHITE.r,
                           WHITE.g,
                           WHITE.b,
                           WHITE.a);
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        error(SDL_GetError);
    }

    if (TTF_Init() < 0) {
        error(TTF_GetError);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        error(Mix_GetError);
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
            SCREEN_WIDTH * .1f,
            (SCREEN_HEIGHT >> 1) - (PADDLE_HEIGHT >> 1),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };
    pong_paddle ai_paddle = {
            SCREEN_WIDTH - (SCREEN_WIDTH * .1f + PADDLE_WIDTH),
            (SCREEN_HEIGHT >> 1) - (PADDLE_HEIGHT >> 1),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
    };

    SDL_Rect player_rect = {
            (SCREEN_WIDTH >> 1) - (SCORE_WIDTH + TEXT_OFFSET),
            0,
            SCORE_WIDTH,
            SCORE_HEIGHT
    };

    score_box player = {
            &player_rect,
            font,
            0
    };

    SDL_Rect ai_rect = {
            (SCREEN_WIDTH >> 1) + TEXT_OFFSET,
            0,
            SCORE_WIDTH,
            SCORE_HEIGHT
    };

    score_box ai = {
            &ai_rect,
            font,
            0
    };

    Mix_Chunk *wall_bounce;
    Mix_Chunk *paddle_bounce;
    Mix_Chunk *score_sound;

    wall_bounce = Mix_LoadWAV(WALL_BOUNCE);

    if (!wall_bounce) {
        error(Mix_GetError);
    }

    paddle_bounce = Mix_LoadWAV(PADDLE_BOUNCE);

    if (!paddle_bounce) {
        error(Mix_GetError);
    }

    score_sound = Mix_LoadWAV(SCORE_SOUND);

    if (!score_sound) {
        error(Mix_GetError);
    }

    sound_effects sounds = {
            wall_bounce,
            paddle_bounce,
            score_sound
    };

    game_state game = {
            &ball,
            &player_paddle,
            &ai_paddle,
            0, 0,
            &player,
            &ai,
            random_bool() ? 1 : -1,
            random_bool() ? 1 : -1,
            &sounds
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

    // Close open resources
    Mix_FreeChunk(sounds.score_sound);
    Mix_FreeChunk(sounds.wall_bounce);
    Mix_FreeChunk(sounds.paddle_bounce);
    TTF_CloseFont(font);
}

static void
close(void) {
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}

int
main(int argc, char **argv) {
    run();
    close();
    return EXIT_SUCCESS;
}
