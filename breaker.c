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
update(breaker_game *game) {
    //TODO implement
}

static void
error(const char*(*error_function)(void)) {
    fprintf(stderr, "Error: %s\n", error_function());
    exit(EXIT_FAILURE);
}

static void
run(void) {
    //TODO implement resource init, game state, game loop
    int8_t running = true;
}

static void
init(void) {
    //TODO implement SDL Lib initializations, error checks
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
