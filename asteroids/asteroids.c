//
// Created by bryan on 8/27/15.
//

#include <stdlib.h>
#include <common.h>
#include <stdio.h>
#include "asteroids.h"


internal asteroids_game*
init_game(void) {
    //TODO allocate game memory
    asteroids_game *game = malloc(sizeof(asteroids_game));
}

internal void
free_game(asteroids_game *game) {
    //TODO free allocated game assets
    free(game);
}

internal void
run(asteroids_game *game) {
    //TODO implement game loop logic
    printf("Running!\n");
}

internal void
shutdown(void) {
    //TODO close all SDL resources
}


int
main(int argc, char** argv) {
    asteroids_game *game = init_game();
    run(game);
    free_game(game);
    shutdown();
    return EXIT_SUCCESS;
}