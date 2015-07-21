//
// Created by bryan on 7/19/15.
//
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 750
#define SCREEN_HEIGHT 750
#define BALL_SIZE 25
#define MAX_VARIANCE 5
#define true 1
#define false 0

static uint8_t running = true;

typedef struct Ball {
    int x;
    int y;
    int radius;
} Ball;

static inline int8_t
get_variance() {
    return rand() % MAX_VARIANCE;
}

static void
draw_ball(SDL_Renderer *renderer, Ball *ball) {
    int x = ball->radius;
    int y = 0;
    int decision_over_2 = 1 - x;

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, x + ball->x, y + ball->y);
        SDL_RenderDrawPoint(renderer, -x + ball->x, y + ball->y);
        SDL_RenderDrawPoint(renderer, -x + ball->x, -y + ball->y);
        SDL_RenderDrawPoint(renderer, x + ball->x, -y + ball->y);
        SDL_RenderDrawPoint(renderer, -y + ball->x, x + ball->y);
        SDL_RenderDrawPoint(renderer, -y + ball->x, -x + ball->y);
        SDL_RenderDrawPoint(renderer, y + ball->x, x + ball->y);
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
fill_ball(SDL_Renderer *renderer, Ball *ball) {
    // Using Midpoint circle algorithm
    int x = ball->radius;
    int y = 0;
    int decision_over_2 = 1 - x;

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    while (x >= y) {
        SDL_RenderDrawLine(renderer,
                           x + ball->x,
                           y + ball->y,
                           -x + ball->x,
                           y + ball->y);

        SDL_RenderDrawLine(renderer,
                           -x + ball->x,
                           -y + ball->y,
                           x + ball->x,
                           -y + ball->y);

        SDL_RenderDrawLine(renderer,
                           -y + ball->x,
                           x + ball->y,
                           -y + ball->x,
                           -x + ball->y);

        SDL_RenderDrawLine(renderer,
                           y + ball->x,
                           x + ball->y,
                           y + ball->x,
                           -x + ball->y);

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
update(Ball *ball, int base_speed) {
    static int dir_x = 1;
    static int dir_y = 1;
    int base_speed_x = base_speed + get_variance();
    int base_speed_y = base_speed + get_variance();

    // Check left
    if (ball->x <= ball->radius) {
        ball->x = ball->radius;
        dir_x *= -1;
    }

    // Check right
    if (ball->x >= SCREEN_WIDTH - ball->radius) {
        ball->x = SCREEN_WIDTH - ball->radius;
        dir_x *= -1;
    }

    // Check Top
    if (ball->y <= ball->radius) {
        ball->y = ball->radius;
        dir_y *= -1;
    }

    // Check Bottom
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        ball->y = SCREEN_HEIGHT - ball->radius;
        dir_y *= -1;
    }

    // Update speed
    ball->x += base_speed_x * dir_x;
    ball->y += base_speed_y * dir_y;
}

static void
render(SDL_Renderer *renderer, Ball *ball) {
    // Clear screen to red
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    fill_ball(renderer, ball);
    draw_ball(renderer, ball);
}

static void
handle_event(SDL_Event *event, uint8_t *running) {
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        default:
            break;
    }
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
    Ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE
    };

    uint32_t then;
    uint32_t now = SDL_GetTicks();

    // Main Loop
    while (running) {

        then = now;
        now = SDL_GetTicks();

        //poll events
        while(SDL_PollEvent(&event) != 0) {
            handle_event(&event, &running);
        }

        update(&ball, now - then);
        render(renderer, &ball);

        // Update Screen
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
