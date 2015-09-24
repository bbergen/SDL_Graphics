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
#include <random.h>
#include "asteroids.h"
#include "explosion.h"


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
bullet_asteroid_collisions(list *explosions,
                           vector asteroids,
                           vector bullets,
                           Mix_Chunk *expn,
                           screen scrn) {

    if (!bullets || !asteroids) {
        return;
    }

    if (vector_empty(asteroids) || vector_empty(bullets)) {
        return;
    }

    // we need to store the locations to generate small asteroids at.
    // we can't add to the asteroid vector while iterating over it
    vector points = vector_init(vector_size(asteroids));
    int i;
    for (i = 0; i < vector_size(asteroids); i++) {
        asteroid a = vector_get(asteroids, i);
        int j;
        for (j = 0; j < vector_size(bullets); j++) {
            bullet b = vector_get(bullets, j);
            if (asteroid_contains(a, bullet_location(b))) {
                if (asteroid_type(a) == LARGE) {
                    // mark this location to generate small asteroids
                    point *pnt = malloc(sizeof(point));
                    *pnt = asteroid_location(a);
                    vector_add(points, pnt);
                }
                // remove old asteroid
                explode(a);
                remove_bullet(b);
                play_sound_effect(FREE_CHANNEL, expn, 0);
                // create explosion
                point p = asteroid_location(a);
                explosion e = generate_explosion(p.x, p.y, scrn);
                list_add(explosions, e);
            }
        }
    }

    // now iterate over marked locations generating 2 small asteroids each
    for (i = 0; i < vector_size(points); i++) {
        point *p = vector_get(points, i);
        vector_add(asteroids, generate_asteroid(p->x, p->y, scrn, SMALL));
        vector_add(asteroids, generate_asteroid(p->x, p->y, scrn, SMALL));
        if (random_bool()) {
            vector_add(asteroids, generate_asteroid(p->x, p->y, scrn, SMALL));
        }
        // free the point
        free(p);
    }
    // free the point list
    vector_free(points);
}

internal void
prune_asteroids(asteroids_game *game) {

    vector transfer = vector_init(BASE_ASTEROIDS);
    int i;
    // populate transfer list
    for (i = 0; i < vector_size(game->asteroids); i++) {
        if (asteroid_visible(vector_get(game->asteroids, i))) {
            vector_add(transfer, vector_get(game->asteroids, i));
        } else {
            free_asteroid(vector_get(game->asteroids, i));
        }
    }

    // free the old list
    vector_free(game->asteroids);

    // transfer and populate the extra room
    game->asteroids = transfer;
    for (i = vector_size(transfer); i < BASE_ASTEROIDS; i++) {
        point p = random_asteroid_coordinate(*game->scrn);
        vector_add(game->asteroids, generate_asteroid(p.x, p.y, *game->scrn, LARGE));
    }
}

internal int8_t
below_asteroid_threshold(vector asteroids) {
    int visible = 0;
    int i;
    for (i = 0; i < vector_size(asteroids); i++) {
        if (asteroid_visible(vector_get(asteroids, i))) {
            visible++;
        }
    }
    return visible < ASTEROID_THRESHOLD;
}

internal void
update(asteroids_game *game) {
    update_ship(game->current_ship, *game->keys, *game->scrn);
    int i;
    for (i = 0; i < vector_size(game->asteroids); i++) {
        update_asteroid(vector_get(game->asteroids, i), *game->scrn);
    }
    list_for_each(game->explosions, update_explosion);

    //check collisions
    bullet_asteroid_collisions(game->explosions,
                               game->asteroids,
                               visible_bullets(game->current_ship),
                               *((Mix_Chunk**) map_get(game->sounds, SOUND_EXPLOSION_1)),
                               *game->scrn);

    if (below_asteroid_threshold(game->asteroids)) {
        prune_asteroids(game);
    }
    update_sounds(game);
}

internal void
render(SDL_Renderer *renderer, asteroids_game *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderClear(renderer);

    //TODO do drawing here
    int i;
    for (i = 0; i < vector_size(game->asteroids); i++) {
        render_asteroid(renderer, vector_get(game->asteroids, i));
    }
    list_for_each_with_param(game->explosions, render_explosion, renderer);
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
        Mix_Chunk *explosion1 = Mix_LoadWAV(SOUND_EXPLOSION_1);
        map_put(game->sounds, SOUND_SHIP_THRUSTER, &thrust, sizeof(Mix_Chunk*));
        map_put(game->sounds, SOUND_SHIP_SHOOT, &shoot, sizeof(Mix_Chunk*));
        map_put(game->sounds, SOUND_EXPLOSION_1, &explosion1, sizeof(Mix_Chunk*));
    }
}

internal point
random_asteroid_coordinate(screen scrn) {

    point p = {};

    if (random_bool()) {
        // generate X first
        int x = (int) random_in_range(0, scrn.width);
        int y_range = (int) (scrn.height * .4);
        int lower_y = (int) random_in_range(0, y_range);
        int upper_y = (int) random_in_range(scrn.height - y_range, scrn.height);
        p.x = x;
        p.y = random_bool() ? lower_y : upper_y;
    } else {
        // generate Y first
        int y = (int) random_in_range(0, scrn.height);
        int x_range = (int) (scrn.width * .4);
        int lower_x = (int) random_in_range(0, x_range);
        int upper_x = (int) random_in_range(scrn.width - x_range, scrn.width);
        p.y = y;
        p.x = random_bool() ? lower_x : upper_x;
    }
    return p;
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
        Mix_FreeChunk(*((Mix_Chunk**) map_get(game->sounds, SOUND_EXPLOSION_1)));
    }
    map_free(game->sounds);
    int i;
    for (i = 0; i < vector_size(game->asteroids); i++) {
        free_asteroid(vector_get(game->asteroids, i));
    }
    vector_free(game->asteroids);
    list_free(game->explosions);
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
                case SDLK_w:
                case SDLK_UP:
                    game->keys->up_down = true;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    game->keys->down_down = true;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    game->keys->left_down = true;
                    break;
                case SDLK_d:
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
                case SDLK_w:
                case SDLK_UP:
                    game->keys->up_down = false;
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    game->keys->down_down = false;
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    game->keys->left_down = false;
                    break;
                case SDLK_d:
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
    init_sounds(game);
    SDL_GetWindowSize(window, &game->scrn->width, &game->scrn->height);
    game->current_ship = allocate_ship(game->scrn->width >> 1,
                                       game->scrn->height >> 1,
                                       play_lazer_sound,
                                       *((Mix_Chunk**) map_get(game->sounds, SOUND_SHIP_SHOOT)));

    game->asteroids = vector_init(BASE_ASTEROIDS);
    int i;
    for (i = 0; i < BASE_ASTEROIDS; i++) {
        point p = random_asteroid_coordinate(*game->scrn);
        vector_add(game->asteroids, generate_asteroid(p.x, p.y, *game->scrn, LARGE));
    }
    game->explosions = malloc(sizeof(list));
    list_init(game->explosions, explosion_size(), free_explosion);
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

    display_starting_menu(renderer, game);

    // init sound, allocate game objects
    reset_game_state(window, game);

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
            game = init_game();
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