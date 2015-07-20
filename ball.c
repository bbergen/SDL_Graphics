//
// Created by bryan on 7/19/15.
//
#include <SDL2/SDL.h>

static void
update(void) {
    //updates the location of the ball on the screen
}

static void
render(SDL_Renderer *renderer) {
    //renders the ball and background on the screen
}

int
main(int argc, char **argv) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Could Not Initialize Video: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }


    printf("Initial Commit!\n");
    return EXIT_SUCCESS;
}
