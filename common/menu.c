//
// Created by bryan on 8/12/15.
//

#include <SDL2/SDL_mixer.h>
#include "menu.h"
#include "map.h"

#define MENU_CHANGE "resources/sounds/common/menu_change.wav"
#define MENU_SELECT "resources/sounds/common/menu_select.wav"

typedef struct _menu {
    int item_count;
    char **menu_items;
    char *title;
    callback_function callback;
    void *arg;
    SDL_Color *bg;
    SDL_Color *fg;
    SDL_Rect *bounds;
    TTF_Font *font;
    TTF_Font *selected_font;
    TTF_Font *title_font;
    SDL_Renderer *renderer;
    int selected_index;
    int8_t menu_running;
    int8_t return_status;
    Mix_Chunk *menu_change;
    Mix_Chunk *menu_select;
} _menu;

internal SDL_Color
lighten_color(SDL_Color color, double factor) {
    SDL_Color new_color = {
            (uint8_t) MIN(255, color.r + 255 * factor),
            (uint8_t) MIN(255, color.g + 255 * factor),
            (uint8_t) MIN(255, color.b + 255 * factor),
            0x00
    };
    return new_color;
}

menu
init_menu(int item_count,
          callback_function callback,
          char **menu_items,
          SDL_Color *bg,
          SDL_Color *fg,
          SDL_Rect *bounds) {

    _menu *m = malloc(sizeof(_menu));
    m->menu_items = malloc(sizeof(char*) * item_count);

    int i;
    for (i = 0; i < item_count; ++i) {
        m->menu_items[i] = malloc(MENU_ITEM_SIZE);
        strcpy(m->menu_items[i], menu_items[i]);
    }

    m->bg = malloc(sizeof(SDL_Color));
    m->fg = malloc(sizeof(SDL_Color));
    m->bounds = malloc(sizeof(SDL_Rect));

    // build
    m->item_count = item_count;
    m->callback = callback;
    *m->bg = *bg;
    *m->fg = *fg;
    *m->bounds = *bounds;
    m->title_font = NULL;

    return m;
}

internal void
render_menu(_menu *m) {

    // clear screen
    SDL_SetRenderDrawColor(m->renderer, m->bg->r, m->bg->g, m->bg->b, m->bg->a);
    SDL_RenderClear(m->renderer);

    SDL_Surface *title_surface = TTF_RenderText_Blended(m->title_font, m->title, *m->fg);
    SDL_Texture *title_texture = SDL_CreateTextureFromSurface(m->renderer, title_surface);
    SDL_Rect title_rect = {
            (m->bounds->w >> 1) - (title_surface->w >> 1),
            (int) (m->bounds->h * .05),
            title_surface->w,
            title_surface->h
    };
    SDL_RenderCopy(m->renderer, title_texture, NULL, &title_rect);
    SDL_FreeSurface(title_surface);

    int i;
    SDL_Rect item_bounds;
    int menu_y = (int) (m->bounds->h * .3);
    for (i = 0; i < m->item_count; ++i) {
        char *menu_item = m->menu_items[i];
        SDL_Color menu_item_color = i == m->selected_index ? lighten_color(*m->fg, .5) : *m->fg;
        TTF_Font *render_font = i == m->selected_index ? m->selected_font :m->font;
        SDL_Surface *surface = TTF_RenderText_Blended(render_font, menu_item, menu_item_color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(m->renderer, surface);
        item_bounds.x = (m->bounds->w >> 1) - (surface->w >> 1);
        item_bounds.y = menu_y;
        item_bounds.w = surface->w;
        item_bounds.h = surface->h;
        menu_y += surface->h;
        SDL_RenderCopy(m->renderer, texture, NULL, &item_bounds);
        SDL_FreeSurface(surface);
    }

    // display menu
    SDL_RenderPresent(m->renderer);
}

internal void
play_menu_effect(Mix_Chunk *chunk) {
    Mix_PlayChannel(-1, chunk, 0);
}

internal void
process_menu_event(_menu *m, SDL_Event *event) {
    int8_t render = false;
    callback_function callback = m->callback;
    switch (event->type) {
        case SDL_QUIT:
            m->menu_running = false;
            m->return_status = QUIT_FROM_MENU;
            break;
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_SPACE:
                case SDLK_ESCAPE:
                case SDLK_PAUSE:
                case SDLK_RETURN: {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    play_menu_effect(m->menu_select);
                    m->menu_running = callback(renderer, m->selected_index, &m->menu_items[m->selected_index], m->arg);
                    render = true;
                } break;
                case SDLK_UP:
                    play_menu_effect(m->menu_change);
                    m->selected_index--;
                    if (m->selected_index < 0) {
                        m->selected_index = m->item_count - 1;
                    }
                    render = true;
                    break;
                case SDLK_DOWN:
                    play_menu_effect(m->menu_change);
                    m->selected_index++;
                    m->selected_index %= m->item_count;
                    render = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_WINDOWEVENT:
            render = true;
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        default:
            break;
    }

    if (render) {
        render_menu(m);
    }
}

int8_t
display_menu(SDL_Renderer *renderer, menu m, const char *font_file, const char *title, void *callback_arg) {

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_Init(SDL_INIT_AUDIO);
    }

    if (!TTF_WasInit()) {
        TTF_Init();
    }

    SDL_Event event;
    _menu *mnu = m;
    mnu->menu_running = true;
    mnu->renderer = renderer;
    mnu->title = title;
    mnu->font = TTF_OpenFont(font_file, 50);
    mnu->selected_font = TTF_OpenFont(font_file, 75);
    if (!mnu->title_font) {
        mnu->title_font = TTF_OpenFont(font_file, 100);
    }
    mnu->arg = callback_arg;
    mnu->selected_index = 0;
    mnu->return_status = 1;
    mnu->menu_change = Mix_LoadWAV(MENU_CHANGE);
    mnu->menu_select = Mix_LoadWAV(MENU_SELECT);

    render_menu(m);

    // menu loop
    while (mnu->menu_running) {
        while (SDL_PollEvent(&event) != 0) {
            process_menu_event(mnu, &event);
        }
    }

    Mix_FreeChunk(mnu->menu_change);
    Mix_FreeChunk(mnu->menu_select);
    TTF_CloseFont(mnu->font);
    TTF_CloseFont(mnu->selected_font);
    TTF_CloseFont(mnu->title_font);
    return mnu->return_status;
}

void
destroy_menu(menu m) {
    _menu *menu_to_free = m;
    int i;
    for (i = 0; i < menu_to_free->item_count; ++i) {
        free(menu_to_free->menu_items[i]);
    }
    free(menu_to_free->menu_items);
    free(menu_to_free->bg);
    free(menu_to_free->fg);
    free(menu_to_free->bounds);
    free(menu_to_free);
}

void
init_title_font(menu m, const char* font_file) {
    if (!m || !font_file) {
        return;
    }

    _menu *mnu = m;
    mnu->title_font = TTF_OpenFont(font_file, 100);
}