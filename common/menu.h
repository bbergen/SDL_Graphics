//
// Created by bryan on 8/12/15.
//

#ifndef SDL_GRAPHICS_MENU_H
#define SDL_GRAPHICS_MENU_H

#include <SDL2/SDL.h>
#include "common.h"

typedef void (*callback_function) (void *, void *);

typedef struct menu {
    int item_count;
    char **menu_items;
    callback_function *call_backs;
    SDL_Color *bg;
    SDL_Color *fg;
    SDL_Rect *bounds;
} menu;

menu*
init_menu(int menu_count,
          callback_function *call_backs,
          char **menu_items,
          SDL_Color *bg,
          SDL_Color *bg,
          SDL_Rect *bounds);

void
display_menu(SDL_Renderer *renderer, menu *m);

void
destroy_menu(menu *m);

#endif //SDL_GRAPHICS_MENU_H
