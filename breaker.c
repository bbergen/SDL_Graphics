//
// Created by bryan on 7/24/15.
//
#include "breaker.h"
#include "common.h"

static void
render_brick(SDL_Renderer *renderer, breaker_brick *brick) {
    //TODO implement
}

static void
render_ball(SDL_Renderer *renderer, breaker_ball *ball) {
    //TODO implement
}

static void
render_paddle(SDL_Renderer *renderer, breaker_paddle *paddle) {
    //TODO implement
}

static void
render(SDL_Renderer *renderer, breaker_game *game) {
    //TODO implement
}

static void
update(breaker_game *game, float delta_t) {
    //TODO implement
}

static void
process_event(SDL_Event *event, int8_t *running) {
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
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

    breaker_ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE
    };

    breaker_game game = {
            &ball
    };

    int8_t running = true;
    SDL_Event event;
    uint32_t then;
    uint32_t now = SDL_GetTicks();
    float delta_t;

    //game loop
    while (running) {
        then = now;
        now = SDL_GetTicks();
        delta_t = (now - then) / 1000.0f;

        while (SDL_PollEvent(&event) !=0) {
            process_event(&event, &running);
        }

        update(&game, delta_t);
        render(renderer, &game);
    }
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
