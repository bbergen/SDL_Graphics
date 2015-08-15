//
// Created by bryan on 8/12/15.
//

#ifndef SDL_GRAPHICS_MENU_H
#define SDL_GRAPHICS_MENU_H

#include <SDL2/SDL.h>
#include "common.h"

static const int8_t QUIT_FROM_MENU = -1;

/*
 * Callback called when menu item activated
 * first arg is the menu item, second arg is an optional dynamic parameter
 */
typedef void (*callback_function) (void *, void *);

/*
 * Menu implementation hidden behind interface
 */
typedef void* menu;

/*
 * Creates and returns a menu based on the passed parameters.
 * There should be one callback per menu item (not necessarily unique).
 */
menu init_menu(int, callback_function*, char**, SDL_Color*, SDL_Color*, SDL_Rect*);

/*
 * Displays the menu.
 * Activating any menu item will run the associated callback
 * then dispose of the menu
 *
 * Third parameter is the optional argument passed to the callback functions
 *
 * returns QUIT_FROM_MENU if the application should close, NULL otherwise
 */
int8_t display_menu(SDL_Renderer*, menu, void*);

/*
 * Free the memory associate with the passed menu
 */
void destroy_menu(menu);

#endif //SDL_GRAPHICS_MENU_H
