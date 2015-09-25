//
// Created by bryan on 8/16/15.
//

#ifndef SDL_GRAPHICS_COLORS_H
#define SDL_GRAPHICS_COLORS_H

#include <SDL2/SDL.h>
#include "common.h"

/*
 * ANSI Console Colors
 */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGNETA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/*
 * SDL Color Constants
 */
global const SDL_Color WHITE = {0xFF, 0xFF, 0xFF, 0x00};

global const SDL_Color PURPLE = {0x4B, 0x00, 0x82, 0x00};

global const SDL_Color BLACK = {0x00, 0x00, 0x00, 0x00};

global const SDL_Color BLUE = {0x14, 0xAD, 0xDB, 0x00};

global const SDL_Color GREEN = {0x2F, 0xB0, 0x04, 0x00};

global const SDL_Color RED = {0xFF, 0x00, 0x00, 0x00};

global const SDL_Color SCREEN = {0xAE, 0xC6, 0xCF, 0xFF};

global const SDL_Color DARK_GREY = {0x69, 0x69, 0x69, 0x00};

global const SDL_Color YELLOW = {0xFF, 0xFF, 0x00, 0x00};

global const SDL_Color PALE_YELLOW = {0xFF, 0xFF, 0x66, 0x00};

global const SDL_Color PALE_ORANGE = {0xFF, 0xA5, 0x00, 0x00};

global const SDL_Color DARK_ORANGE = {0xFF, 0x45, 0x00, 0x00};

#endif //SDL_GRAPHICS_COLORS_H
