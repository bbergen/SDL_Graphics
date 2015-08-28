//
// Created by bryan on 8/27/15.
//

#include <stdlib.h>
#include <common.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL.h>
#include <util.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <colors.h>
#include <menu.h>
#include "asteroids.h"


internal int8_t
asteroids_menu_callback(SDL_Renderer *renderer, int index, char **menu_item, void *param) {
    return true;
}

internal void
update(asteroids_game *game) {
    //TODO update game objects
}

internal void
render(SDL_Renderer *renderer, asteroids_game *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderClear(renderer);

    //TODO do drawing here

    // present renderer
    SDL_RenderPresent(renderer);
}

internal keyboard*
init_keys(void) {
    keyboard *k = malloc(sizeof(keyboard));
    k->left_down = false;
    k->right_down = false;
    k->up_down = false;
    k->down_down = false;
    return k;
}

internal asteroids_game*
init_game(void) {
    asteroids_game *game = malloc(sizeof(asteroids_game));
    game->current_ship = allocate_ship(SCREEN_WIDTH >> 1, SCREEN_HEIGHT >> 1);
    game->keys = init_keys();
    game->running = true;
    game->event = malloc(sizeof(SDL_Event));
    game->scrn = malloc(sizeof(screen));
    return game;
}

internal void
free_game(asteroids_game *game) {
    free_ship(game->current_ship);
    free(game->keys);
    free(game->scrn);
    free(game->event);
    free(game);
}

internal void
process_event(asteroids_game *game) {
    switch (game->event->type) {
        case SDL_QUIT:
            game->running = false;
            break;
        case SDL_KEYDOWN:
            switch (game->event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    game->running = false;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}


internal void
run(asteroids_game *game) {

    SDL_Window *window = SDL_CreateWindow("Asteroids!",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        error(SDL_GetError);
    }

    //TODO change to a user preference later on
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GetWindowSize(window, &game->scrn->width, &game->scrn->height);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error(SDL_GetError);
    }

    char *menu_items[MENU_ITEM_SIZE];
    menu_items[0] = "New Game";
    menu_items[1] = "Options";
    menu_items[2] = "About Asteroids";
    menu_items[3] = "Quit";

    SDL_Rect menu_rect = {};
    menu_rect.x = 0;
    menu_rect.y = 0;
    menu_rect.w = game->scrn->width;
    menu_rect.h = game->scrn->height;

    menu m = init_menu(4, asteroids_menu_callback, menu_items, &BLACK, &BLUE, &menu_rect);

    if (display_menu(renderer, m, "resources/fonts/built_tiling.ttf", "Asteroids!", NULL) == QUIT_FROM_MENU) {
        exit(EXIT_SUCCESS);
    }
    destroy_menu(m);

    //game loop
    while (game->running) {
        while(SDL_PollEvent(game->event) != 0) {
            process_event(game);
        }
        update(game);
        render(renderer, game);
    }
}

internal void
shutdown(void) {
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

internal void
init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        error(SDL_GetError);
    }

    if (TTF_Init() < 0) {
        error(TTF_GetError);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        error(IMG_GetError);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        error(Mix_GetError);
    }
}

int
main(int argc, char** argv) {
    init_sdl();
    asteroids_game *game = init_game();
    run(game);
    free_game(game);
    shutdown();
    return EXIT_SUCCESS;
}