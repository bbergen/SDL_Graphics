//
// Created by bryan on 7/24/15.
//
#include "breaker.h"

static int8_t
render_brick(void *b) {
    breaker_brick *brick = b;
    if (brick->visible) {
        SDL_Color *color = brick->color;
        SDL_SetRenderDrawColor(brick->renderer, color->r, color->g, color->b, color->a);
        SDL_Rect rect = {
                brick->x,
                brick->y,
                brick->width,
                brick->height
        };
        SDL_RenderFillRect(brick->renderer, &rect);
        SDL_SetRenderDrawColor(brick->renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderDrawRect(brick->renderer, &rect);
    }
    return true;
}

static point
circle_intersect(int angle, int x, int y, int radius) {
    double theta = angle * (180 / PI);

    point p = {};
    p.x = (int) (x + radius * cos(theta));
    p.y = (int) (y + radius * sin(theta));

    return p;
}

static int8_t
has_brick_ball_collided(void *brick_arg, void *ball_arg) {
    breaker_brick *brick = (breaker_brick*) brick_arg;
    breaker_ball *ball = (breaker_ball*) ball_arg;

    if (!brick->visible) {
        return true;
    }

    int8_t has_collided = false;
    static int SIZE = 2;

    // collision boxes
    SDL_Rect top_right = { brick->x, brick->y, SIZE, SIZE};
    SDL_Rect top = {brick->x + SIZE, brick->y, brick->width - SIZE * 2, SIZE};
    SDL_Rect top_left = {brick->x + brick->width - SIZE, brick->y, SIZE, SIZE};
    SDL_Rect left = { brick->x, brick->y + SIZE, SIZE, brick->height - SIZE * 2};
    SDL_Rect right = { brick->x + brick->width - SIZE, brick->y + SIZE, SIZE, brick->height - SIZE * 2};
    SDL_Rect bottom_left = {brick->x, brick->y + brick->height - SIZE, SIZE, SIZE};
    SDL_Rect bottom_right = {brick->x + brick->width - SIZE, brick->y + brick->height - SIZE, SIZE, SIZE};
    SDL_Rect bottom = {brick->x + SIZE, brick->y + brick->height - SIZE, brick->width - SIZE * 2, SIZE};

    int ball_x = (int)ball->x;
    int ball_y = (int)ball->y;
    int end_x;
    int end_y;
    point p;

    p = circle_intersect(255, ball_x, ball_y, ball->radius);
    if (SDL_IntersectRectAndLine(&top_right, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    end_x = ball_x;
    end_y = ball_y + ball->radius;
    if (!has_collided && SDL_IntersectRectAndLine(&top, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->y_dir *= -1;
    }

    p = circle_intersect(315, ball_x, ball_y, ball->radius);
    if (!has_collided && SDL_IntersectRectAndLine(&top_left, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    end_x = ball_x + ball->radius;
    end_y = ball_y;
    if (!has_collided && SDL_IntersectRectAndLine(&left, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->x_dir *= -1;
    }

    end_x = ball_x - ball->radius;
    end_y = ball_y;
    if (!has_collided && SDL_IntersectRectAndLine(&right, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->x_dir *= -1;
    }

    p = circle_intersect(45, ball_x, ball_y, ball->radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom_left, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    p = circle_intersect(135, ball_x, ball_y, ball->radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom_right, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    end_x = ball_x;
    end_y = ball_y - ball->radius;
    if (!has_collided && SDL_IntersectRectAndLine(&bottom, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->y_dir *= -1;
    }

    brick->visible = !has_collided;
    return !has_collided;
}

static void
random_color(SDL_Color *color) {
    uint8_t red = (uint8_t) (rand() % 255);
    uint8_t blue = (uint8_t) (rand() % 255);
    uint8_t green = (uint8_t) (rand() % 255);
    SDL_Color c = {
            red, blue, green, 0x00
    };
    *color = c;
}

static void
build_brick(breaker_brick *brick, SDL_Renderer *renderer, int x, int y, int width, int height) {
    brick->x = x;
    brick->y = y;
    brick->width = width;
    brick->height = height;
    brick->renderer = renderer;
    brick->visible = true;
    brick->color = malloc(sizeof(SDL_Color));
    random_color(brick->color);
}

static void
free_brick(void *b) {
    breaker_brick *brick = b;
    free(brick->color);
}

static void
build_brick_list(list *l, SDL_Renderer *renderer, int bricks) {
    init_list(l, sizeof(breaker_brick), free_brick);

    int step = 0;
    int x = 0;
    for (int i = 0; i < bricks; i++) {

        if (i % 12 == 0) {
            step++;
            x = 0;
        }

        int y = (int) (SCREEN_HEIGHT * 0.1f) + (step * BRICK_HEIGHT);
        int width = BRICK_WIDTH;
        int height = BRICK_HEIGHT;
        breaker_brick brick;
        build_brick(&brick, renderer, x, y, width, height);
        add(l, &brick);
        x += BRICK_WIDTH;

    }
}

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

    //render bricks
    list_for_each(game->brick_list, render_brick);

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

    // check brick collisions
    list_for_each_with_param(game->brick_list,
                             has_brick_ball_collided,
                             game->ball);

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

    list brick_list;
    build_brick_list(&brick_list, renderer, 60);

    breaker_game game = {
            &ball,
            &paddle,
            NULL,
            &sounds,
            false,
            false,
            &brick_list
    };

    int8_t running = true;
    SDL_Event event;
    uint32_t then;
    uint32_t now = SDL_GetTicks();
    float delta_t;

    //start music
    start_music(game.sounds->music);

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
    free_list(game.brick_list);
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
    Mix_Quit();
    SDL_Quit();
}

int
main(int argc, char** argv) {
    init();
    run();
    close();
    return EXIT_SUCCESS;
}
