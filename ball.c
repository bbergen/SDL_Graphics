//
// Created by bryan on 7/19/15.
//
#include <SDL2/SDL.h>
#define true 1
#define false 0

static const int SCREEN_WIDTH = 750;
static const int SCREEN_HEIGHT = 750;
static int running = true;

static void
update(void) {
    //updates the location of the ball on the screen
}

static void
render(SDL_Renderer *renderer) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Draw (Red for now)
    SDL_Rect screen = {
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &screen);

    // Update Screen
    SDL_RenderPresent(renderer);
}

int
main(int argc, char **argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Could Not Initialize Video: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Bouncing Ball!",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "SDL Could Not Initialize Window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        fprintf(stderr, "SDL Could Not Initialize Renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Event event;

    // Main Loop
    while (running) {

        //poll events
        while(!SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                default:
                    break;
            }

            update();
            render(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
