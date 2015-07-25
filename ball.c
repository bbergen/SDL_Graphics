//
// Created by bryan on 7/19/15.
//
#include <SDL2/SDL.h>
#include "common.h"

#define SCREEN_WIDTH 750
#define SCREEN_HEIGHT 750
#define BALL_SIZE 25
#define VARIANCE 0.025f
#define BALL_SPEED 200

static uint8_t running = true;

typedef struct pong_ball {
    float x;
    float y;
    int radius;
} pong_ball;

static void
draw_ball(SDL_Renderer *renderer, pong_ball *ball) {
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
fill_ball(SDL_Renderer *renderer, pong_ball *ball) {
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
update(pong_ball *ball, float speed) {
    static int dir_x = 1;
    static int dir_y = 1;
    float speed_x = speed * BALL_SPEED;
    float speed_y = speed * BALL_SPEED;

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
    ball->x += speed_x * dir_x;
    ball->y += speed_y * dir_y + VARIANCE * dir_y;
}

static void
render(SDL_Renderer *renderer, pong_ball *ball) {
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

    SDL_Window *window = SDL_CreateWindow("Bouncing pong_ball!",
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
    pong_ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE
    };

    uint32_t then;
    uint32_t now = SDL_GetTicks();
    float delta;

    // Main Loop
    while (running) {

        then = now;
        now = SDL_GetTicks();
        delta = (now - then) / 1000.0f;

        //poll events
        while(SDL_PollEvent(&event) != 0) {
            handle_event(&event, &running);
        }

        update(&ball, delta);
        render(renderer, &ball);

        // Update Screen
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
