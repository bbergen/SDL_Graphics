//
// Created by bryan on 7/19/15.
//
#include <SDL2/SDL.h>
#define true 1
#define false 0

static const int SCREEN_WIDTH = 750;
static const int SCREEN_HEIGHT = 750;
static const int BALL_SIZE = 25;
static int running = true;

struct Ball {
    int x;
    int y;
    int radius;
};

static void
draw_ball(SDL_Renderer *renderer, struct Ball *ball) {
    // Using Midpoint circle algorithm
    //TODO make this fill as well as draw
    int x = ball->radius;
    int y = 0;
    int decision_over_2 = 1 - x;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, x + ball->x, y + ball->y);
        SDL_RenderDrawPoint(renderer, y + ball->x, x + ball->y);
        SDL_RenderDrawPoint(renderer, -x + ball->x, y + ball->y);
        SDL_RenderDrawPoint(renderer, -y + ball->x, x + ball->y);
        SDL_RenderDrawPoint(renderer, -x + ball->x, -y + ball->y);
        SDL_RenderDrawPoint(renderer, -y + ball->x, -x + ball->y);
        SDL_RenderDrawPoint(renderer, x + ball->x, -y + ball->y);
        SDL_RenderDrawPoint(renderer, y + ball->x, -x + ball->y);
        y++;
        if (decision_over_2 <= 0) {
            decision_over_2 += 2 * y + 1;
        } else {
            x--;
            decision_over_2 += 2 * (y - x) + 1;
        }
    }
}

static void
update(struct Ball *ball) {
    static int speed_x = 1;
    static int speed_y = 2;

    // Check left
    if (ball->x <= ball->radius) {
        speed_x *= -1;
    }

    // Check right
    if (ball->x >= SCREEN_WIDTH - ball->radius) {
        speed_x *= -1;
    }

    // Check Top
    if (ball->y <= ball->radius) {
        speed_y *= -1;
    }

    // Check Bottom
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        speed_y *= -1;
    }

    // Update speed
    ball->x += speed_x;
    ball->y += speed_y;
}

static void
render(SDL_Renderer *renderer, struct Ball *ball) {
    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    // Draw (Red for now)
    SDL_Rect screen = {
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT
    };
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(renderer, &screen);
    draw_ball(renderer, ball);

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
    struct Ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE
    };

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

            update(&ball);
            render(renderer, &ball);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
