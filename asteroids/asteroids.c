//
// Created by bryan on 8/27/15.
//

#include <stdlib.h>
#include <common.h>
#include <stdio.h>
#include "asteroids.h"


internal keyboard*
init_keys(void) {
    keyboard *k = malloc(sizeof(keyboard));
    k->left_down = false;
    k->right_down = false;
    k->up_down = false;
    k->down_down = false;
    return k;
}

internal void
free_keys(keyboard *keys) {
    free(keys);
}

internal asteroids_game*
init_game(void) {
    asteroids_game *game = malloc(sizeof(asteroids_game));
    game->current_ship = allocate_ship(SCREEN_WIDTH >> 1, SCREEN_HEIGHT >> 1);
    game->keys = init_keys();
    return game;
}

internal void
free_game(asteroids_game *game) {
    free_ship(game->current_ship);
    free_keys(game->keys);
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