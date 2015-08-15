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

void
display_menu(SDL_Renderer *renderer, menu m) {
    //TODO implement
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