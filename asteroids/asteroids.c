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


internal int
play_sound_effect(int channel, Mix_Chunk *chunk, int loops) {
    return Mix_PlayChannel(channel, chunk, loops);
}

internal void
update_sounds(asteroids_game *game) {
    persistent int thrust_channel = FREE_CHANNEL;
    if (ship_thrusting(game->current_ship) && !Mix_Playing(thrust_channel)) {
        Mix_Chunk *chunk = *((Mix_Chunk**) map_get(game->sounds, SOUND_SHIP_THRUSTER));
        thrust_channel = play_sound_effect(thrust_channel, chunk, -1);
    } else if (!ship_thrusting(game->current_ship) &&
            thrust_channel != FREE_CHANNEL &&
            Mix_Playing(thrust_channel)) {
        Mix_HaltChannel(thrust_channel);
        thrust_channel = FREE_CHANNEL;
    }
}

internal int8_t
options_menu_callback(SDL_Renderer *renderer, int index, char **menu_item, void *param) {
    //TODO implement options, such as sound, music and resolution
    return index != 3 && index != 4;
}

internal void
display_options_menu(SDL_Renderer *renderer, asteroids_game *game) {
    //TODO implement options, such as sound, music and resolution
    char *menu_items[MENU_ITEM_SIZE];
    menu_items[0] = "Resolution - ???? x ????";
    menu_items[1] = "Sound On";
    menu_items[2] = "Music On";
    menu_items[3] = "Save";
    menu_items[4] = "Return";

    SDL_Rect menu_rect = {};
    menu_rect.x = 0;
    menu_rect.y = 0;
    menu_rect.w = game->scrn->width;
    menu_rect.h = game->scrn->height;

    menu m = init_menu(5, options_menu_callback, menu_items, &BLACK, &BLUE, &menu_rect);
    init_title_font(m, FONT_ASTEROIDS_TITLE);

    if (display_menu(renderer, m, FONT_ASTEROIDS_MENU, "Options", game) == QUIT_FROM_MENU) {
        free_game(game);
        shutdown();
        exit(EXIT_SUCCESS);
    }
    destroy_menu(m);
}

internal int8_t
asteroids_menu_callback(SDL_Renderer *renderer, int index, char **menu_item, void *param) {
    asteroids_game *game = param;
    switch (index) {
        case 0: // new game
            return false;
        case 1: // options
            display_options_menu(renderer, game);
            return true;
        case 2: // about
            return true;
        case 3: // quit
            free_game(game);
            shutdown();
            exit(EXIT_SUCCESS);
        default:
            return true;
    }
}

internal int8_t
asteroids_paused_menu_callback(SDL_Renderer *renderer, int index, char **menu_item, void *param) {
    asteroids_game *game = param;
    switch (index) {
        case 0: // continue
            return false;
        case 1: // new game
            game->game_over = true;
            return false;
        case 2: // options
            display_options_menu(renderer, game);
            return true;
        case 3: // about
            return true;
        case 4: // quit
        default:
            free_game((asteroids_game*)param);
            shutdown();
            exit(EXIT_SUCCESS);
    }
}

internal void
display_starting_menu(SDL_Renderer *renderer, asteroids_game *game) {
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
    init_title_font(m, FONT_ASTEROIDS_TITLE);

    if (display_menu(renderer, m, FONT_ASTEROIDS_MENU, "Asteroids", game) == QUIT_FROM_MENU) {
        free_game(game);
        shutdown();
        exit(EXIT_SUCCESS);
    }
    destroy_menu(m);
}

internal void
display_pause_menu(SDL_Renderer *renderer, asteroids_game *game) {
    char *menu_items[MENU_ITEM_SIZE];
    menu_items[0] = "Continue";
    menu_items[1] = "New Game";
    menu_items[2] = "Options";
    menu_items[3] = "About Asteroids";
    menu_items[4] = "Quit";

    SDL_Rect menu_rect = {};
    menu_rect.x = 0;
    menu_rect.y = 0;
    menu_rect.w = game->scrn->width;
    menu_rect.h = game->scrn->height;

    menu m = init_menu(5, asteroids_paused_menu_callback, menu_items, &BLACK, &BLUE, &menu_rect);
    init_title_font(m, FONT_ASTEROIDS_TITLE);

    if (display_menu(renderer, m, FONT_ASTEROIDS_MENU, "Paused", game) == QUIT_FROM_MENU) {
        free_game(game);
        shutdown();
        exit(EXIT_SUCCESS);
    }
    destroy_menu(m);
}

internal void
update(asteroids_game *game) {
    update_ship(game->current_ship, *game->keys, *game->scrn);
    update_sounds(game);
}

internal void
render(SDL_Renderer *renderer, asteroids_game *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderClear(renderer);

    //TODO do drawing here
    render_ship(renderer, game->current_ship);

    // present renderer
    SDL_RenderPresent(renderer);
}

internal void
init_sounds(asteroids_game *game) {
    if (game->sounds) {
        // we need to map only the pointer to the chunk pointer so that we can properly free later
        Mix_Chunk *thrust = Mix_LoadWAV(SOUND_SHIP_THRUSTER);
        Mix_Chunk *shoot = Mix_LoadWAV(SOUND_SHIP_SHOOT);
        map_put(game->sounds, SOUND_SHIP_THRUSTER, &thrust, sizeof(Mix_Chunk*));
        map_put(game->sounds, SOUND_SHIP_SHOOT, &shoot, sizeof(Mix_Chunk*));
    }
}

internal asteroids_game*
init_game(void) {
    asteroids_game *game = malloc(sizeof(asteroids_game));
    game->keys = calloc(1, sizeof(keyboard)); //keys must be zeroed
    game->running = true;
    game->game_over = false;
    game->event = malloc(sizeof(SDL_Event));
    game->scrn = malloc(sizeof(screen));
    game->sounds = map_init();
    return game;
}

internal void
free_game(asteroids_game *game) {
    free_ship(game->current_ship);
    if (game->sounds) {
        // sound map contains only pointers to chunk pointers for proper freeing
        Mix_FreeChunk(*((Mix_Chunk**) map_get(game->sounds, SOUND_SHIP_THRUSTER)));
        Mix_FreeChunk(*((Mix_Chunk**) map_get(game->sounds, SOUND_SHIP_SHOOT)));
    }
    map_free(game->sounds);
    free(game->keys);
    free(game->scrn);
    free(game->event);
    free(game);
}

internal void
clear_keys(keyboard **keys) {
    if (*keys) {
        free(*keys);
        *keys = calloc(1, sizeof(keyboard));
    }
}

internal void
process_event(asteroids_game *game) {
    switch (game->event->type) {
        case SDL_QUIT:
            game->running = false;
            break;
        case SDL_KEYDOWN:
            switch (game->event->key.keysym.sym) {
                case SDLK_PAUSE:
                case SDLK_ESCAPE: {
                    SDL_Window *window = SDL_GetWindowFromID(game->event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    thrusting_state(game->current_ship, false);
                    update_sounds(game);
                    display_pause_menu(renderer, game);
                    clear_keys(&game->keys);
                } break;
                case SDLK_UP:
                    game->keys->up_down = true;
                    break;
                case SDLK_DOWN:
                    game->keys->down_down = true;
                    break;
                case SDLK_LEFT:
                    game->keys->left_down = true;
                    break;
                case SDLK_RIGHT:
                    game->keys->right_down = true;
                    break;
                case SDLK_SPACE:
                    game->keys->space_down = true;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (game->event->key.keysym.sym) {
                case SDLK_UP:
                    game->keys->up_down = false;
                    break;
                case SDLK_DOWN:
                    game->keys->down_down = false;
                    break;
                case SDLK_LEFT:
                    game->keys->left_down = false;
                    break;
                case SDLK_RIGHT:
                    game->keys->right_down = false;
                    break;
                case SDLK_SPACE:
                    game->keys->space_down = false;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

internal void
play_lazer_sound(void *sound) {
    Mix_Chunk *shot = sound;
    play_sound_effect(FREE_CHANNEL, shot, 0);
}

internal void
reset_game_state(SDL_Window *window, asteroids_game *game) {
    game = init_game();
    init_sounds(game);
    SDL_GetWindowSize(window, &game->scrn->width, &game->scrn->height);
    game->current_ship = allocate_ship(game->scrn->width >> 1,
                                       game->scrn->height >> 1,
                                       play_lazer_sound,
                                       *((Mix_Chunk**) map_get(game->sounds, SOUND_SHIP_SHOOT)));
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
//    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GetWindowSize(window, &game->scrn->width, &game->scrn->height);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error(SDL_GetError);
    }

    init_sounds(game);
    display_starting_menu(renderer, game);

    // initialize game objects
    game->current_ship = allocate_ship(game->scrn->width >> 1,
                                       game->scrn->height >> 1,
                                       play_lazer_sound,
                                       *((Mix_Chunk**) map_get(game->sounds, SOUND_SHIP_SHOOT)));

    uint32_t then;
    uint32_t now = SDL_GetTicks();
    const uint64_t update_freq = 1000 / 60;
    double ticks_passed = 0.0;

    //game loop
    while (game->running) {
        then = now;
        now = SDL_GetTicks();
        ticks_passed += now - then;

        while(SDL_PollEvent(game->event) != 0) {
            process_event(game);
        }

        if (game->game_over) {
            free_game(game);
            reset_game_state(window, game);
        }

        if (ticks_passed >= update_freq) {
            update(game);
            ticks_passed = 0.0;
        }

        render(renderer, game);
    }
    SDL_DestroyRenderer(renderer);
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