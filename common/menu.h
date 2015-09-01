//
// Created by bryan on 8/12/15.
//

#ifndef SDL_GRAPHICS_MENU_H
#define SDL_GRAPHICS_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "common.h"

#define MENU_ITEM_SIZE 50

static const int8_t QUIT_FROM_MENU = -1;

/*
 * Callback called when menu item activated
 * arg 1: Index of the selected menu item
 * arg 2: The renderer associated with the current window
 * arg 3: Pointer to the text of the menu item
 * arg 4: optional parameter passed via init_menu
 * return: anything but 0 if the menu should continue displaying
 */
typedef int8_t (*callback_function) (SDL_Renderer *renderer, int, char**, void*);

/*
 * Menu implementation hidden behind interface
 */
typedef void* menu;

/*
 * Creates and returns a menu based on the passed parameters.
 * arg 1: number of menu items
 * arg 2: pointer to function called on menu item activation
 * arg 3: menu items
 * arg 4: menu background
 * arg 5: menu foreground
 * arg 6: menu bounds
 *
 * return: an initialized menu, ready to be displayed using display_menu
 */
menu init_menu(int, callback_function, char**, SDL_Color*, SDL_Color*, SDL_Rect*);

/*
 * Displays the menu.
 * arg 1: Renderer used to render the menu
 * arg 2: menu created from init_menu
 * arg 3: font file path used to display menu
 * arg 4: optional title for the menu
 * arg 5: optional generic pointer to pass to the callback function
 *
 * return: QUIT_FROM_MENU if the application should close, NULL otherwise
 */
int8_t display_menu(SDL_Renderer*, menu, const char*, const char*, void*);

/*
 * Free the memory associate with the passed menu
 */
void destroy_menu(menu);

/*
 * Sets a separate font for use if the title has been set
 * arg 1: menu created from init_menu
 * arg 2: font file path for the menu title
 */
void init_title_font(menu, const char*);

#endif //SDL_GRAPHICS_MENU_H
