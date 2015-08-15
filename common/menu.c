//
// Created by bryan on 8/12/15.
//

#include "menu.h"
#include "map.h"

typedef struct _menu {
    int item_count;
    char **menu_items;
    hash_map *item_callbacks;
    SDL_Color *bg;
    SDL_Color *fg;
    SDL_Rect *bounds;
} _menu;

typedef struct menu_data {
    _menu *m;
    int8_t menu_running;
    int8_t return_status;
    SDL_Renderer *renderer;
    SDL_Event *event;
    int selected_index;
    void *callback_arg;
} menu_data ;

menu
init_menu(int item_count,
          callback_function *call_backs,
          char **menu_items,
          SDL_Color *bg,
          SDL_Color *fg,
          SDL_Rect *bounds) {

    _menu *m = malloc(sizeof(_menu));
    hash_map menu_map = init_map();
    m->menu_items = malloc(sizeof(char*) * item_count);

    int i;
    for (i = 0; i < item_count; ++i) {
        size_t key_size = strlen(menu_items[i] + 1);
        m->menu_items[i] = malloc(key_size);
        strcpy(m->menu_items[i], menu_items[i]);
        put(menu_map, m->menu_items[i], call_backs++, sizeof(callback_function));
    }

    m->item_callbacks = malloc(sizeof(hash_map));
    m->bg = malloc(sizeof(SDL_Color));
    m->fg = malloc(sizeof(SDL_Color));
    m->bounds = malloc(sizeof(SDL_Rect));

    // build
    m->item_count = item_count;
    *m->item_callbacks = menu_map;
    *m->bg = *bg;
    *m->fg = *fg;
    *m->bounds = *bounds;

    return m;
}

internal void
render_menu(SDL_Renderer *renderer, _menu *m, int selected_index) {

    // clear screen
    SDL_SetRenderDrawColor(renderer, m->bg->r, m->bg->g, m->bg->b, m->bg->a);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, m->fg->r, m->fg->g, m->fg->b, m->fg->a);
    SDL_Rect test_rect = {
            m->bounds->w >> 2,
            m->bounds->h >> 2,
            m->bounds->w >> 1,
            m->bounds->h >> 1
    };
    SDL_RenderFillRect(renderer, &test_rect);

    // display menu
    SDL_RenderPresent(renderer);
}

internal void
process_menu_event(menu_data *data) {
    switch (data->event->type) {
        case SDL_QUIT:
            data->menu_running = false;
            data->return_status = QUIT_FROM_MENU;
            break;
        case SDL_KEYDOWN:
            switch (data->event->key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_ESCAPE:
                case SDLK_PAUSE:
                case SDLK_RETURN:
                    data->menu_running = false;
                    break;
                default:
                    break;
            }
            break;
        case SDL_WINDOWEVENT:
            render_menu(data->renderer, data->m, data->selected_index);
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        default:
            break;
    }
}

int8_t
display_menu(SDL_Renderer *renderer, menu m, void *callback_arg) {

    SDL_Event event;
    menu_data data = {
            m,
            true,
            NULL,
            renderer,
            &event,
            0,
            callback_arg
    };

    // menu loop
    while (data.menu_running) {
        while (SDL_PollEvent(&event) != 0) {
            process_menu_event(&data);
        }
    }
    return data.return_status;
}

void
destroy_menu(menu m) {
    _menu *menu_to_free = m;
    int i;
    for (i = 0; i < menu_to_free->item_count; ++i) {
        free(menu_to_free->menu_items[i]);
    }
    free(menu_to_free->menu_items);
    free_map(*menu_to_free->item_callbacks);
    free(menu_to_free->item_callbacks);
    free(menu_to_free->bg);
    free(menu_to_free->fg);
    free(menu_to_free->bounds);
    free(menu_to_free);
}