//
// Created by bryan on 7/24/15.
//
#include <SDL2/SDL_image.h>
#include <x86intrin.h>
#include <list.h>
#include <util.h>
#include "menu.h"
#include "breaker.h"

internal void
play_sound_effect(Mix_Chunk *effect) {
    Mix_PlayChannel(-1, effect, 0);
}

internal int8_t
contains_point(point *p, SDL_Rect *rect) {
    int8_t x_bounds = p->x > rect->x && p->x < rect->x + rect->w;
    int8_t y_bounds = p->y > rect->y && p->y < rect->y + rect->h;
    return x_bounds && y_bounds;
}

internal int8_t
render_brick(void *b) {
    breaker_brick *brick = b;
    if (brick->visible) {
        SDL_Color *color = brick->color;
        SDL_SetRenderDrawColor(brick->renderer, color->r, color->g, color->b, color->a);
        SDL_Rect rect = {
                brick->x,
                brick->y,
                brick->width,
                brick->height
        };
        SDL_RenderFillRect(brick->renderer, &rect);
        SDL_SetRenderDrawColor(brick->renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
        SDL_RenderDrawRect(brick->renderer, &rect);
    }
    return true;
}

internal point
circle_intersect(int angle, int x, int y, int radius) {
    double theta = angle * (180 / PI);

    point p = {};
    p.x = (int) (x + radius * cos(theta));
    p.y = (int) (y + radius * sin(theta));

    return p;
}

internal int8_t
has_brick_ball_collided(void *brick_arg, void *game_arg) {
    breaker_brick *brick = brick_arg;
    breaker_ball *ball = ((breaker_game*) game_arg)->ball;

    if (!brick->visible) {
        return true;
    }

    int8_t has_collided = false;
    persistent int SIZE = 2;

    // collision boxes
    SDL_Rect top_right = { brick->x, brick->y, SIZE, SIZE};
    SDL_Rect top = {brick->x + SIZE, brick->y, brick->width - SIZE * 2, SIZE};
    SDL_Rect top_left = {brick->x + brick->width - SIZE, brick->y, SIZE, SIZE};
    SDL_Rect left = { brick->x, brick->y + SIZE, SIZE, brick->height - SIZE * 2};
    SDL_Rect right = { brick->x + brick->width - SIZE, brick->y + SIZE, SIZE, brick->height - SIZE * 2};
    SDL_Rect bottom_left = {brick->x, brick->y + brick->height - SIZE, SIZE, SIZE};
    SDL_Rect bottom_right = {brick->x + brick->width - SIZE, brick->y + brick->height - SIZE, SIZE, SIZE};
    SDL_Rect bottom = {brick->x + SIZE, brick->y + brick->height - SIZE, brick->width - SIZE * 2, SIZE};

    int ball_x = (int)ball->x;
    int ball_y = (int)ball->y;
    int end_x;
    int end_y;
    point p;

    p = circle_intersect(255, ball_x, ball_y, ball->radius);
    if (SDL_IntersectRectAndLine(&top_right, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    end_x = ball_x;
    end_y = ball_y + ball->radius;
    if (!has_collided && SDL_IntersectRectAndLine(&top, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->y_dir *= -1;
    }

    p = circle_intersect(315, ball_x, ball_y, ball->radius);
    if (!has_collided && SDL_IntersectRectAndLine(&top_left, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    end_x = ball_x + ball->radius;
    end_y = ball_y;
    if (!has_collided && SDL_IntersectRectAndLine(&left, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->x_dir *= -1;
    }

    end_x = ball_x - ball->radius;
    end_y = ball_y;
    if (!has_collided && SDL_IntersectRectAndLine(&right, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->x_dir *= -1;
    }

    p = circle_intersect(45, ball_x, ball_y, ball->radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom_left, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    p = circle_intersect(135, ball_x, ball_y, ball->radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom_right, &ball_x, &ball_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->y_dir *= -1;
    }

    end_x = ball_x;
    end_y = ball_y - ball->radius;
    if (!has_collided && SDL_IntersectRectAndLine(&bottom, &ball_x, &ball_y, &end_x, &end_y)) {
        has_collided = true;
        ball->y_dir *= -1;
    }

    if (has_collided && ball->sound_effects_on) {
        play_sound_effect(brick->brick_break);
    }

    if (has_collided) {
        ((breaker_game*) game_arg)->current_score += brick->value;
    }

    brick->visible = !has_collided;
    return !has_collided;
}

internal void
random_color(SDL_Color *color) {
    uint8_t red = (uint8_t) (rand() % 255);
    uint8_t blue = (uint8_t) (rand() % 255);
    uint8_t green = (uint8_t) (rand() % 255);
    SDL_Color c = {
            red, blue, green, 0x00
    };
    *color = c;
}

internal void
build_brick(breaker_brick *brick,
            SDL_Renderer *renderer,
            Mix_Chunk *brick_break,
            SDL_Color color,
            int x, int y, int width, int height, int value) {
    brick->x = x;
    brick->y = y;
    brick->width = width;
    brick->height = height;
    brick->value = value;
    brick->renderer = renderer;
    brick->brick_break = brick_break;
    brick->visible = true;
    brick->color = malloc(sizeof(SDL_Color));
    *brick->color = color;
}

internal void
free_brick(void *b) {
    breaker_brick *brick = b;
    free(brick->color);
}

internal void
build_brick_list(list *l, SDL_Renderer *renderer, Mix_Chunk *brick_break, int bricks) {
    init_list(l, sizeof(breaker_brick), free_brick);

    int step = 0;
    int x = 0;
    SDL_Color color = {};
    random_color(&color);
    for (int i = 0; i < bricks; i++) {

        if (i % 12 == 0) {
            step++;
            x = 0;
            random_color(&color);
        }

        int y = (int) (SCREEN_HEIGHT * 0.2f) + (step * BRICK_HEIGHT);
        int width = BRICK_WIDTH;
        int height = BRICK_HEIGHT;
        breaker_brick brick;
        build_brick(&brick, renderer, brick_break, color, x, y, width, height, 5);
        add(l, &brick);
        x += BRICK_WIDTH;

    }
}

internal void
start_music(Mix_Music *track) {
    Mix_PlayMusic(track, -1);
}

internal void
pause_music(void) {
    if (Mix_PlayingMusic()) {
        if (Mix_PausedMusic()) {
            Mix_ResumeMusic();
        } else {
            Mix_PauseMusic();
        }
    }
}

internal void
render_circle(SDL_Renderer *renderer, const SDL_Color *color, int mid_x, int mid_y, int radius) {
    int x = radius;
    int y = 0;
    int decision_over_2 = 1 - x;

    SDL_SetRenderDrawColor(renderer,
                           color->r,
                           color->g,
                           color->b,
                           color->a);

    while (x >= y) {
        SDL_RenderDrawLine(renderer,
                           x + mid_x,
                           y + mid_y,
                           -x + mid_x,
                           y + mid_y);

        SDL_RenderDrawLine(renderer,
                           -x + mid_x,
                           -y + mid_y,
                           x + mid_x,
                           -y + mid_y);

        SDL_RenderDrawLine(renderer,
                           -y + mid_x,
                           x + mid_y,
                           -y + mid_x,
                           -x + mid_y);

        SDL_RenderDrawLine(renderer,
                           y + mid_x,
                           x + mid_y,
                           y + mid_x,
                           -x + mid_y);

        y++;
        if (decision_over_2 <= 0) {
            decision_over_2 += 2 * y + 1;
        } else {
            x--;
            decision_over_2 += 2 * (y - x) + 1;
        }
    }
}

internal void
render_ball(SDL_Renderer *renderer, breaker_ball *ball) {
    render_circle(renderer, &PURPLE, (int) ball->x, (int) ball->y, ball->radius);
}

internal void
render_paddle(SDL_Renderer *renderer, breaker_paddle *paddle) {
    SDL_Rect rect = {
            (int) paddle->x, (int) paddle->y, paddle->width, paddle->height
    };
    SDL_SetRenderDrawColor(renderer, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, &rect);
}

internal void
render_label(SDL_Renderer *renderer, label *l) {
    SDL_Rect bounds = {l->location.x, l->location.y, l->text_width, l->text_height};
    SDL_RenderCopy(renderer, l->text, NULL, &bounds);
}

internal int
find_mid_point(int width, int bounds) {
    return (bounds >> 1) - (width >> 1);
}

internal void
render_component(SDL_Renderer *renderer, component *c, int8_t alt_icon) {
    SDL_Texture *icon = alt_icon ? c->alt_icon : c->icon;
    SDL_Rect inset_rect = {
            c->bounds->x + c->inset.left,
            c->bounds->y + c->inset.top,
            c->bounds->w - (c->inset.left + c->inset.right),
            c->bounds->h - (c->inset.top + c->inset.bottom)
    };
    SDL_RenderCopy(renderer, icon, NULL, &inset_rect);
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, c->bounds);
    render_label(renderer, c->component_label);
}

internal void
update_text_field(SDL_Renderer *renderer, component *c, TTF_Font *font, SDL_Color *color, int updated_value) {
    char string[SCORE_PADDING + 1];
    itoa(updated_value, string, SCORE_PADDING);
    SDL_Surface *new_surface = TTF_RenderText_Blended(font, string, *color);
    SDL_Texture *new_texture = SDL_CreateTextureFromSurface(renderer, new_surface);
    c->icon = new_texture;
    SDL_FreeSurface(new_surface);
}

internal void
update_score_and_lives(SDL_Renderer *renderer, score_box *box, int current_score, int current_lives) {

    persistent int score = STARTING_SCORE;
    persistent int lives = STARTING_LIVES;

    if (current_score > score) {
        score = current_score;
        update_text_field(renderer, box->current_score_field, box->text_font, &GREEN, current_score);
    }

    if (current_lives > lives) {
        lives = current_lives;
        update_text_field(renderer, box->lives_field, box->text_font, &RED, current_lives);
    }

}

internal void
render_score_box(SDL_Renderer *renderer, score_box *box, int current_score, int current_lives) {

    update_score_and_lives(renderer, box, current_score, current_lives);

    int score_box_width = SCREEN_WIDTH - (SCORE_OFFSET<< 1);

    //draw outline
    SDL_Rect outline = {
            SCORE_OFFSET, SCORE_OFFSET, score_box_width, CEILING - 4
    };
    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderFillRect(renderer, &outline);
    SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderDrawRect(renderer, &outline);

    //draw ceiling line
    SDL_RenderDrawLine(renderer, 0, CEILING, SCREEN_WIDTH, CEILING);

    //draw music button
    render_component(renderer, box->music_button, !box->music_on);

    //draw sound button
    render_component(renderer, box->sound_button, !box->sound_on);

    //draw high score box
    render_component(renderer, box->high_score_field, false);

    //draw current score box
    render_component(renderer, box->current_score_field, false);

    //draw lives box
    render_component(renderer, box->lives_field, false);
}

internal void
render(SDL_Renderer *renderer, breaker_game *game) {
    // clear screen
    SDL_SetRenderDrawColor(renderer,
                           0xAE,
                           0xC6,
                           0xCF,
                           0xFF);
    //TODO make this constant color if I want to keep it.
    SDL_RenderClear(renderer);

    // render game objects
    render_ball(renderer, game->ball);
    render_paddle(renderer, game->player);
    render_score_box(renderer, game->score, game->current_score, game->lives);

    // render bricks
    list_for_each(game->brick_list, render_brick);

    // present backbuffer
    SDL_RenderPresent(renderer);
}

internal int8_t
check_paddle_collisions(breaker_paddle *paddle, breaker_ball *ball) {

    persistent int CORNER_HEIGHT = 3;
    persistent int CORNER_WIDTH = 15;

    int p_x = (int) paddle->x;
    int p_y = (int) paddle->y;
    int p_width = paddle->width;
    int p_height = paddle->height;

    int b_x = (int) ball->x;
    int b_y = (int) ball->y;
    int radius = ball->radius;

    SDL_Rect top_left = {p_x, p_y, CORNER_WIDTH, CORNER_HEIGHT};
    SDL_Rect top = {p_x + CORNER_WIDTH, p_y, p_width - CORNER_WIDTH * 2, CORNER_HEIGHT};
    SDL_Rect top_right = {p_x + p_width - CORNER_WIDTH, p_y, CORNER_WIDTH, CORNER_HEIGHT};
    SDL_Rect left = {p_x, p_y + CORNER_HEIGHT, CORNER_HEIGHT, p_height - CORNER_HEIGHT * 2};
    SDL_Rect right = {p_x + p_width - CORNER_WIDTH, p_y + CORNER_HEIGHT, CORNER_HEIGHT, p_height - CORNER_HEIGHT * 2};
    SDL_Rect bottom_left = {p_x, p_y + p_height - CORNER_HEIGHT, CORNER_WIDTH, CORNER_HEIGHT};
    SDL_Rect bottom = {p_x + CORNER_WIDTH, p_y + p_height - CORNER_HEIGHT, p_width - CORNER_WIDTH * 2, CORNER_HEIGHT};
    SDL_Rect bottom_right = {p_x + p_width - CORNER_WIDTH, p_y + p_height - CORNER_HEIGHT, CORNER_WIDTH, CORNER_HEIGHT};

    int8_t has_collided = false;
    point p;

    p = circle_intersect(315, b_x, b_y, radius);
    if (SDL_IntersectRectAndLine(&top_left, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->y_dir *= -1;
        ball->x_dir = - 1; //always bounce left
        ball->y = top_left.y - ball->radius;
    }

    p = circle_intersect(270, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&top, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->y_dir *= -1;
        ball->y = top.y - ball->radius;
    }

    p = circle_intersect(225, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&top_right, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->y_dir *= -1;
        ball->x_dir = 1; // always bounce right
        ball->y = top_right.y - ball->radius;
    }

    p = circle_intersect(360, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&left, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->x = p_x - ball->radius;
    }

    p = circle_intersect(180, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&right, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->x_dir *= -1;
        ball->x = p_x + p_width + ball->radius;
    }

    p = circle_intersect(45, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom_left, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->y_dir *= -1;
        ball->x_dir = -1;
    }

    p = circle_intersect(90, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->y_dir *= -1;
    }

    p = circle_intersect(135, b_x, b_y, radius);
    if (!has_collided && SDL_IntersectRectAndLine(&bottom_right, &b_x, &b_y, &p.x, &p.y)) {
        has_collided = true;
        ball->y_dir *= -1;
        ball->x_dir = 1;
    }

    return has_collided;
}

internal void
update_ball(breaker_game *game) {
    float speed_x = STARTING_SPEED;
    float speed_y = STARTING_SPEED;
    breaker_ball *ball = game->ball;
    ball->sound_effects_on = game->score->sound_on;

    // check left
    if (ball->x <= ball->radius) {
        ball->x = ball->radius;
        ball->x_dir *= -1;
        if (game->ball->sound_effects_on) {
            play_sound_effect(game->sounds->wall_bounce);
        }
    }

    // check right
    if (ball->x >= SCREEN_WIDTH - ball->radius) {
        ball->x = SCREEN_WIDTH - ball->radius;
        ball->x_dir *= -1;
        if (game->ball->sound_effects_on) {
            play_sound_effect(game->sounds->wall_bounce);
        }
    }

    // check top
    if (ball->y <= ball->radius + CEILING) {
        ball->y = ball->radius + CEILING;
        ball->y_dir *= -1;
        if (game->ball->sound_effects_on) {
            play_sound_effect(game->sounds->wall_bounce);
        }
    }

    // check bottom
    if (ball->y >= SCREEN_HEIGHT - ball->radius) {
        ball->y = SCREEN_HEIGHT - ball->radius;
        ball->y_dir *= -1;
        if (game->ball->sound_effects_on) {
            play_sound_effect(game->sounds->wall_bounce);
        }
    }

    // check brick collisions
    list_for_each_with_param(game->brick_list,
                             has_brick_ball_collided,
                             game);

    // check paddle collision
    if (check_paddle_collisions(game->player, ball)) {
        if (game->ball->sound_effects_on) {
            play_sound_effect(game->sounds->wall_bounce);
        }
    }


    // update position
    ball->x += speed_x * ball->x_dir;
    ball->y += speed_y * ball->y_dir;
}

internal void
update_paddle(breaker_paddle *paddle, int8_t right_down, int8_t left_down) {

    if (right_down) {
        if (paddle->x <= SCREEN_WIDTH - PADDLE_WIDTH) {
            paddle->x += STARTING_SPEED;
        } else {
            paddle->x = SCREEN_WIDTH - PADDLE_WIDTH;
        }
    }

    if (left_down) {
        if (paddle->x >= 0) {
            paddle->x -= STARTING_SPEED;
        } else {
            paddle->x = 0;
        }
    }
}

internal void
init_score_box(score_box *box, SDL_Surface **surfaces) {
    int score_box_width = SCREEN_WIDTH - (SCORE_OFFSET<< 1);
    int field_size = (score_box_width - ((BUTTON_SIZE * 3) + SCORE_OFFSET * 6)) >> 1;
    int x = SCORE_OFFSET << 1;
    int y = SCORE_OFFSET << 1;

    SDL_Rect button = {
            SCORE_OFFSET << 1,
            CEILING - (SCORE_OFFSET + BUTTON_SIZE),
            BUTTON_SIZE,
            BUTTON_SIZE
    };

    label lbl = *box->music_button->component_label;
    point music_point = {x + find_mid_point(lbl.text_width, BUTTON_SIZE), y};
    box->music_button->component_label->location = music_point;
    *box->music_button->bounds = button;

    lbl = *box->sound_button->component_label;
    button.x += BUTTON_SIZE + SCORE_OFFSET;
    x += BUTTON_SIZE + SCORE_OFFSET;
    point sound_point = {x + find_mid_point(lbl.text_width, BUTTON_SIZE), y};
    box->sound_button->component_label->location = sound_point;
    *box->sound_button->bounds = button;

    lbl = *box->high_score_field->component_label;
    button.x += BUTTON_SIZE + SCORE_OFFSET;
    button.w = field_size;
    x += BUTTON_SIZE + SCORE_OFFSET;
    point high_score_point = {x + find_mid_point(lbl.text_width, field_size), y};
    box->high_score_field->component_label->location = high_score_point;
    *box->high_score_field->bounds = button;
    SDL_Surface *high_score_surface = *surfaces++;
    insets high_score_insets = {
            (field_size - high_score_surface->w) >> 1,
            (field_size - high_score_surface->w) >> 1,
            (BUTTON_SIZE - high_score_surface->h) >> 1,
            (BUTTON_SIZE - high_score_surface->h) >> 1
    };
    box->high_score_field->inset = high_score_insets;

    lbl = *box->current_score_field->component_label;
    button.x += field_size + SCORE_OFFSET;
    x += field_size + SCORE_OFFSET;
    point current_score_point = {x + find_mid_point(lbl.text_width, field_size), y};
    box->current_score_field->component_label->location = current_score_point;
    *box->current_score_field->bounds = button;
    SDL_Surface *current_score_surface = *surfaces++;
    insets current_score_insets = {
            (field_size - current_score_surface->w) >> 1,
            (field_size - current_score_surface->w) >> 1,
            (BUTTON_SIZE - current_score_surface->h) >> 1,
            (BUTTON_SIZE - current_score_surface->h) >> 1
    };
    box->current_score_field->inset = current_score_insets;

    lbl = *box->lives_field->component_label;
    button.x += field_size + SCORE_OFFSET;
    button.w = BUTTON_SIZE;
    x += field_size + SCORE_OFFSET;
    point lives_point = {x + find_mid_point(lbl.text_width, BUTTON_SIZE), y};
    box->lives_field->component_label->location = lives_point;
    *box->lives_field->bounds = button;
    SDL_Surface *lives_surface = *surfaces;
    insets lives_insets = {
            (BUTTON_SIZE - lives_surface->w) >> 1,
            (BUTTON_SIZE - lives_surface->w) >> 1,
            (BUTTON_SIZE - lives_surface->h) >> 1,
            (BUTTON_SIZE - lives_surface->h) >> 1
    };
    box->lives_field->inset = lives_insets;

}

internal void
update_score_box(score_box *box, point *mouse_loc, int8_t mouse_down) {

    persistent uint32_t last_music_ticks = 0;
    persistent uint32_t last_sound_ticks = 0;
    uint32_t ticks = SDL_GetTicks();
    uint32_t music_ticks_passed = ticks - last_music_ticks;
    uint32_t sound_ticks_passed = ticks - last_sound_ticks;
    int8_t music_pressed = contains_point(mouse_loc, box->music_button->bounds);
    int8_t sound_pressed = contains_point(mouse_loc, box->sound_button->bounds);

    if ((!last_music_ticks || SDL_TICKS_PASSED(music_ticks_passed, BUTTON_DELAY))
        && music_pressed && mouse_down) {
        pause_music();
        box->music_on = !box->music_on;
        last_music_ticks = ticks;
    }

    if ((!last_sound_ticks || SDL_TICKS_PASSED(sound_ticks_passed, BUTTON_DELAY))
        && sound_pressed && mouse_down) {
        box->sound_on = !box->sound_on;
        last_sound_ticks = ticks;
    }

}

internal void
update(breaker_game *game) {
    update_score_box(game->score, game->mouse_loc, game->mouse_down);
    update_ball(game);
    update_paddle(game->player, game->key_right_down, game->key_left_down);
}

internal void
process_event(SDL_Event *event, breaker_game * game, int8_t *running) {
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
                case SDLK_RIGHT:
                    game->key_right_down = true;
                    break;
                case SDLK_LEFT:
                    game->key_left_down = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_RIGHT:
                    game->key_right_down = false;
                    break;
                case SDLK_LEFT:
                    game->key_left_down = false;
                    break;
                default:
                    break;
            }
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&game->mouse_loc->x, &game->mouse_loc->y);
            break;
        case SDL_MOUSEBUTTONUP:
            game->mouse_down = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            game->mouse_down = true;
            break;
        default:
            break;
    }
}

internal void
error(const char*(*error_function)(void)) {
    fprintf(stderr, "Error: %s\n", error_function());
    exit(EXIT_FAILURE);
}

internal void
performance_profiling(uint64_t per_count_freq, uint64_t *last_count, uint64_t *last_cycles) {
    uint64_t end_counter = SDL_GetPerformanceCounter();
    uint64_t counter_elapsed = end_counter - *last_count;
    uint64_t end_cycle_count = _rdtsc();
    uint64_t cycles_elapsed = end_cycle_count - *last_cycles;
    double mc_per_frame = ((double) cycles_elapsed / (1000.0f * 1000.0f));
    double ms_per_frame = ((1000.0f * (double) counter_elapsed) / (double) per_count_freq);
    double fps = (double) per_count_freq / (double) counter_elapsed;
    fprintf(stdout, "%.02f ms/f, %.02f f/s, %.02f mc/f\n", ms_per_frame, fps, mc_per_frame);

    *last_count = end_counter;
    *last_cycles = end_cycle_count;
}

internal void
run(void) {
    SDL_Window *window = SDL_CreateWindow("Brick Breaker!",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH,
                                          SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window) {
        error(SDL_GetError);
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, - 1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error(SDL_GetError);
    }

    Mix_Music *level_1 = Mix_LoadMUS(LEVEL_1_TRACK);
    Mix_Chunk *wall_bounce = Mix_LoadWAV(WALL_BOUNCE);
    Mix_Chunk *brick_bounce = Mix_LoadWAV(BRICK_BOUNCE);
    Mix_Chunk *brick_break = Mix_LoadWAV(BRICK_BREAK);

    if (!level_1 || !wall_bounce || !brick_bounce || !brick_break) {
        error(Mix_GetError);
    }

    breaker_sounds sounds = {
            level_1,
            wall_bounce,
            brick_bounce,
    };

    breaker_ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE,
            1, 1,
            true
    };

    breaker_paddle paddle = {
            (SCREEN_WIDTH >> 1) - (PADDLE_WIDTH >> 1),
            SCREEN_HEIGHT * .9f,
            PADDLE_WIDTH,
            PADDLE_HEIGHT,
    };

    list brick_list;
    build_brick_list(&brick_list, renderer, brick_break, 60);

    TTF_Font *label_font = TTF_OpenFont(SCORE_FONT, 18);
    TTF_Font *score_font = TTF_OpenFont(SCORE_FONT, 35);

    if (!label_font || !score_font ) {
        error(TTF_GetError);
    }

    SDL_Surface *music_on = IMG_Load(MUSIC_ON);
    SDL_Surface *music_off = IMG_Load(MUSIC_OFF);
    SDL_Surface *sound_on = IMG_Load(SOUND_ON);
    SDL_Surface *sound_off = IMG_Load(SOUND_OFF);

    if (!music_on || !music_off || !sound_on || !sound_off) {
        error(IMG_GetError);
    }

    SDL_Texture *music_on_icon = SDL_CreateTextureFromSurface(renderer, music_on);
    SDL_Texture *music_off_icon = SDL_CreateTextureFromSurface(renderer, music_off);
    SDL_Texture *sound_on_icon = SDL_CreateTextureFromSurface(renderer, sound_on);
    SDL_Texture *sound_off_icon = SDL_CreateTextureFromSurface(renderer, sound_off);

    if (!music_on_icon || !music_off_icon || !sound_on_icon || !sound_off_icon) {
        error(SDL_GetError);
    }

    SDL_Surface *music_surface = TTF_RenderText_Blended(label_font, "Music", BLACK);
    SDL_Texture *music_texture = SDL_CreateTextureFromSurface(renderer, music_surface);
    SDL_Surface *sound_surface = TTF_RenderText_Blended(label_font, "Sound", BLACK);
    SDL_Texture *sound_texture = SDL_CreateTextureFromSurface(renderer, sound_surface);
    SDL_Surface *high_score_label_surface = TTF_RenderText_Blended(label_font, "High Score", BLACK);
    SDL_Texture *high_score_label_texture = SDL_CreateTextureFromSurface(renderer, high_score_label_surface);
    SDL_Surface *current_score_label_surface = TTF_RenderText_Blended(label_font, "Current Score", BLACK);
    SDL_Texture *current_score_label_texture = SDL_CreateTextureFromSurface(renderer, current_score_label_surface);
    SDL_Surface *lives_label_surface = TTF_RenderText_Blended(label_font, "Lives", BLACK);
    SDL_Texture *lives_label_texture = SDL_CreateTextureFromSurface(renderer, lives_label_surface);

    //score surfaces
    char high_score[SCORE_PADDING + 1];
    itoa(STARTING_SCORE, high_score, SCORE_PADDING);
    SDL_Surface *high_score_surface = TTF_RenderText_Blended(score_font, high_score, GREEN);
    SDL_Texture *high_score_texture = SDL_CreateTextureFromSurface(renderer, high_score_surface);

    char current_score[SCORE_PADDING + 1];
    itoa(STARTING_SCORE, current_score, SCORE_PADDING);
    SDL_Surface *current_score_surface = TTF_RenderText_Blended(score_font, high_score, GREEN);
    SDL_Texture *current_score_texture = SDL_CreateTextureFromSurface(renderer, current_score_surface);

    char lives[3];
    itoa(STARTING_LIVES, lives, 0);
    SDL_Surface *lives_surface = TTF_RenderText_Blended(score_font, lives, RED);
    SDL_Texture *lives_texture = SDL_CreateTextureFromSurface(renderer, lives_surface);

    point music_point = {0,0};
    label music = {
            music_texture, music_point, music_surface->w, music_surface->h
    };

    point sound_point = {0,0};
    label sound = {
            sound_texture, sound_point, sound_surface->w, sound_surface->h
    };

    point high_score_point = {0,0};
    label high_score_label = {
            high_score_label_texture, high_score_point, high_score_label_surface->w, high_score_label_surface->h
    };

    point current_score_point = {0,0};
    label current_score_label = {
            current_score_label_texture, current_score_point, current_score_label_surface->w, current_score_label_surface->h
    };

    point lives_point = {0,0};
    label lives_label = {
            lives_label_texture, lives_point, lives_label_surface->w, lives_label_surface->h
    };

    component music_c = {
            &music,
            music_on_icon,
            music_off_icon,
            malloc(sizeof(SDL_Rect))
    };

    component sound_c = {
            &sound,
            sound_on_icon,
            sound_off_icon,
            malloc(sizeof(SDL_Rect))
    };

    component high_score_c = {
            &high_score_label,
            high_score_texture,
            NULL,
            malloc(sizeof(SDL_Rect))
    };

    component current_score_c = {
            &current_score_label,
            current_score_texture,
            NULL,
            malloc(sizeof(SDL_Rect))
    };

    component lives_c = {
            &lives_label,
            lives_texture,
            NULL,
            malloc(sizeof(SDL_Rect))
    };

    score_box box = {
            &music_c,
            &sound_c,
            &high_score_c,
            &current_score_c,
            &lives_c,
            true,
            true,
            score_font
    };

    SDL_Surface *surfaces[3] = {high_score_surface, current_score_surface, lives_surface};

    init_score_box(&box, surfaces);

    point mouse_loc = {0,0};

    breaker_game game = {
            &ball,
            &paddle,
            &box,
            &sounds,
            false,
            false,
            &brick_list,
            STARTING_LIVES,
            STARTING_SCORE,
            STARTING_SCORE,
            false,
            &mouse_loc
    };

    int8_t running = true;
    SDL_Event event;
    uint32_t then;
    uint32_t now = SDL_GetTicks();
    float delta_t;

    //start music
    start_music(game.sounds->music);

#if DEBUG
    //performance tracking
    uint64_t per_count_frequency = SDL_GetPerformanceFrequency();
    uint64_t last_counter = SDL_GetPerformanceCounter();
    uint64_t last_cycle_count = _rdtsc();
#endif

    const uint64_t update_freq = 1000 / 60;
    double ticks_passed = 0.0;

    //game loop
    while (running) {
        then = now;
        now = SDL_GetTicks();
        ticks_passed += now - then;

        while (SDL_PollEvent(&event) !=0) {
            process_event(&event, &game, &running);
        }

        if (ticks_passed >= update_freq) {
            update(&game);
            ticks_passed -= update_freq;
        }
        render(renderer, &game);

#if DEBUG
        performance_profiling(per_count_frequency, &last_counter, &last_cycle_count);
#endif
    }

    //free resources
    //TODO free more stuff (font textures, etc.)
    free_list(game.brick_list);
    free(music_c.bounds);
    free(sound_c.bounds);
    free(high_score_c.bounds);
    free(current_score_c.bounds);
    free(lives_c.bounds);
    Mix_FreeMusic(level_1);
    Mix_FreeChunk(wall_bounce);
    Mix_FreeChunk(brick_bounce);
    Mix_FreeChunk(brick_break);
    TTF_CloseFont(label_font);
    TTF_CloseFont(score_font);
}

internal void
init(void) {

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        error(SDL_GetError);
    }

    if (TTF_Init() < 0) {
        error(TTF_GetError);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        error(IMG_GetError);
    }

    if (Mix_OpenAudio(AUDIO_FREQUENCY,
                      MIX_DEFAULT_FORMAT,
                      AUDIO_CHANNELS,
                      AUDIO_CHUNK_SIZE) < 0) {
        error(Mix_GetError);
    }

}

internal void
close(void) {
    Mix_Quit();
    SDL_Quit();
}

int
main(int argc, char** argv) {
    init();
    run();
    close();
    return EXIT_SUCCESS;
}
