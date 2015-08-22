//
// Created by bryan on 7/24/15.
//
#include <SDL2/SDL_image.h>
#include <x86intrin.h>
#include <sys/stat.h>
#include "list.h"
#include "util.h"
#include "menu.h"
#include "breaker.h"
#include "colors.h"

internal void
play_sound_effect(Mix_Chunk *effect) {
    Mix_PlayChannel(-1, effect, 0);
}

internal int8_t
random_bool(void) {
    return rand() % 2 == 0;
}

internal SDL_Color
lighten_brick(SDL_Color color, double factor) {
    SDL_Color new_color = {
            (uint8_t) MIN(255, color.r + 255 * factor),
            (uint8_t) MIN(255, color.g + 255 * factor),
            (uint8_t) MIN(255, color.b + 255 * factor),
            0x00
    };
    return new_color;
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
        SDL_Color color;
        switch (brick->type) {
            case TRIPLE:
                color = *brick->base_color;
                break;
            case DOUBLE:
                color = lighten_brick(*brick->base_color, .25);
                break;
            case UNBREAKABLE:
                color = DARK_GREY;
                break;
            case NORMAL:
            default:
                color = lighten_brick(*brick->base_color, .50);
                break;
        }
        SDL_SetRenderDrawColor(brick->renderer, color.r, color.g, color.b, color.a);
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
        if (brick->type == NORMAL) {
            play_sound_effect(brick->brick_break);
        } else {
            play_sound_effect(brick->brick_bounce);
        }
    }

    if (has_collided && brick->type <= 0) {
        breaker_game *game = game_arg;
        double modifier = game->current_level->difficulty_modifier;
        game->current_score += brick->value * modifier;
        game->current_level->bricks_left--;
        brick->visible = false;
    }

    if (has_collided && brick->type != UNBREAKABLE) {
        brick->type--;
    }


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
free_brick(void *b) {
    breaker_brick *brick = b;
    free(brick->base_color);
}

internal void
build_brick_list(SDL_Renderer *renderer, level *l, BRICK_TYPE brick_matrix[MAX_ROWS][LEVEL_WIDTH], int rows) {
    l->brick_list = malloc(sizeof(list));
    init_list(l->brick_list, sizeof(breaker_brick), free_brick);

    SDL_Color color = {};
    random_color(&color);

    Mix_Chunk *brick_break = Mix_LoadWAV(BRICK_BREAK);
    Mix_Chunk *brick_bounce = Mix_LoadWAV(BRICK_BOUNCE);


    int y = (int) (SCREEN_HEIGHT * 0.2);
    int i;
    for (i = 0; i < rows; ++i) {
        int x = 0;
        int j;
        for (j = 0; j < LEVEL_WIDTH; ++j) {
            BRICK_TYPE type = brick_matrix[i][j];
            if (type == SPACE) {
                x += BRICK_WIDTH;
                continue;
            }
            breaker_brick brick;
            brick.x = x;
            brick.y = y;
            brick.width = BRICK_WIDTH;
            brick.height = BRICK_HEIGHT;
            brick.renderer = renderer;
            brick.brick_break = brick_break;
            brick.brick_bounce = brick_bounce;
            brick.visible = true;
            brick.base_color = malloc(sizeof(SDL_Color));
            *brick.base_color = color;
            brick.type = type;
            brick.value = BASE_BRICK_VALUE * (type + 1);

            add(l->brick_list, &brick);
            x += BRICK_WIDTH;
        }
        y += BRICK_HEIGHT;
    }
}

internal void
free_level(level *l) {
    //to free
    Mix_FreeChunk(((breaker_brick*)(l->brick_list->head->data))->brick_break);
    Mix_FreeChunk(((breaker_brick*)(l->brick_list->head->data))->brick_bounce);
    free_list(l->brick_list);
    free(l->brick_list);
    free(l);
}

internal void
build_level(SDL_Renderer *renderer, level *l, const char *csv_file, Mix_Music *music_file) {

    BRICK_TYPE brick_matrix[MAX_ROWS][LEVEL_WIDTH] = {};
    FILE *stream = fopen(csv_file, "r");
    char line[1024];
    int row = 0;
    int breakable_bricks = 0;
    while (fgets(line, 1024, stream)) {
        char *tmp = strdup(line);
        if (tmp[0] == ';') {
            //comment line
            free(tmp);
            continue;
        }

        int column = 0;
        const char *tok;
        for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",")) {
            BRICK_TYPE type;
            switch (tok[0]) {
                case '#':
                    type = UNBREAKABLE;
                    break;
                case '$':
                    type = TRIPLE;
                    breakable_bricks++;
                    break;
                case '%':
                    type = DOUBLE;
                    breakable_bricks++;
                    break;
                case '^':
                    type = NORMAL;
                    breakable_bricks++;
                    break;
                case '_':
                default:
                    type = SPACE;
                    break;
            }
            brick_matrix[row][column++] = type;
        }
        row++;
        free(tmp);
    }
    fclose(stream);

    build_brick_list(renderer, l, brick_matrix, row);
    l->bg = SCREEN;
    l->music = music_file;
    l->bricks_left = breakable_bricks;
}

internal void
save_game_state(breaker_game *game) {

    const int PREF_COUNT = 3;
    char *preferences[PREF_COUNT];
    preferences[0] = PREF_KEY_SCORE;
    preferences[1] = PREF_KEY_MUSIC;
    preferences[2] = PREF_KEY_SOUND;

    int score = game->current_score > game->high_score ? game->current_score : game->high_score;
    int values[PREF_COUNT];
    values[0] = score;
    values[1] = game->score->music_on;
    values[2] = game->score->sound_on;

    char *home = getenv("HOME");
    char path[100] = {};
    strcat(path, home);
    strcat(path, CONFIG_FILE_DIR);
    struct stat st = {};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
    strcat(path, CONFIG_FILE);

    FILE *config_file = fopen(path, "w");
    if (config_file) {
        int i;
        for (i = 0; i < PREF_COUNT; ++i) {
            fprintf(config_file, "%s=%d\n", preferences[i], values[i]);
        }
        fclose(config_file);
    } else {
        //TODO change to logging (Issue #13)
        fprintf(stderr, "Cannot open file: %s", path);
    }
}

internal void
load_game_state(breaker_game *game) {

    char path[100] = {};
    char *home = getenv("HOME");
    strcat(path, home);
    strcat(path, CONFIG_FILE_DIR);
    struct stat st = {};
    if (stat(path, &st) != -1) {
        strcat(path, CONFIG_FILE);
        FILE *config_file = fopen(path, "r");
        if (config_file) {
            char line[100];
            while (fscanf(config_file, "%s", line) != EOF) {
                char *preference = strtok(line, "=");
                char *value_string = strtok(NULL, "=");
                int value = atoi(value_string);
                if (strcmp(preference, PREF_KEY_SCORE) == 0) {
                    game->high_score = value;
                } else if (strcmp(preference, PREF_KEY_MUSIC) == 0) {
                    game->score->music_on = value;
                } else if (strcmp(preference, PREF_KEY_SOUND) == 0) {
                    game->score->sound_on = value;
                    game->ball->sound_effects_on = value;
                }
            }
            fclose(config_file);
        } else {
            //TODO change to logging with (Issue #13)
            fprintf(stderr, "Cannot open file: %s", path);
        }
    }
}

internal void
reset_ball(breaker_ball *ball) {
    ball->x = SCREEN_WIDTH >> 1;
    ball->y = SCREEN_HEIGHT >> 1;
    ball->x_dir = random_bool() ? 1 : -1;
    ball->y_dir = 1;
}

internal void
reset_paddle(breaker_paddle *paddle) {
    paddle->x = (SCREEN_WIDTH >> 1) - (paddle->width >> 1);
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
reset_game(breaker_game *game, SDL_Renderer *renderer, int level_index, int8_t new_level) {

    int current_score = game->current_score;
    int current_lives = game->lives;

    // reset ball
    reset_ball(game->ball);

    // reset paddle
    reset_paddle(game->player);

    // reset level
    if (game->current_level) {
        free_level(game->current_level);
    }
    game->current_level = malloc(sizeof(level));
    build_level(renderer,
                game->current_level,
                game->level_files[level_index],
                game->music_files[level_index]);

    //start music
    start_music(game->current_level->music);
    if (!game->score->music_on) {
        pause_music();
    }

    if (game->current_score > game->high_score) {
        game->high_score = game->current_score;
    }

    save_game_state(game);

    // reset score
    game->current_score = new_level ? current_score : STARTING_SCORE;

    // reset lives
    game->lives = new_level ? current_lives : STARTING_LIVES;

    double modifier;

    if (new_level) {
        modifier = game->current_level->difficulty_modifier + DIFFICULTY_INCREMENT;
    } else {
        modifier = DEFAULT_DIFFICULTY;
    }
    game->current_level->difficulty_modifier = modifier;
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
update_text_field(SDL_Renderer *renderer, component *c, TTF_Font *font, SDL_Color *color, int value, int8_t pad) {
    char string[SCORE_PADDING + 1];
    itoa(value, string, pad ? SCORE_PADDING : 0);
    SDL_Surface *new_surface = TTF_RenderText_Blended(font, string, *color);
    SDL_Texture *new_texture = SDL_CreateTextureFromSurface(renderer, new_surface);
    c->icon = new_texture;
    SDL_FreeSurface(new_surface);
}

internal void
update_score_and_lives(SDL_Renderer *renderer,
                       score_box *box,
                       int high_score,
                       int current_score,
                       int current_lives) {

    persistent int score = STARTING_SCORE;
    persistent int lives = STARTING_LIVES;
    persistent int high = STARTING_SCORE;

    if (high != high_score) {
        high = high_score;
        update_text_field(renderer, box->high_score_field, box->text_font, &GREEN, high_score, true);
    }

    if (current_score != score) {
        score = current_score;
        update_text_field(renderer, box->current_score_field, box->text_font, &GREEN, current_score, true);
    }

    if (current_lives != lives) {
        lives = current_lives;
        update_text_field(renderer, box->lives_field, box->text_font, &RED, current_lives, false);
    }

}

internal void
render_score_box(SDL_Renderer *renderer,
                 score_box *box,
                 int high_score,
                 int current_score,
                 int current_lives) {

    update_score_and_lives(renderer, box, high_score, current_score, current_lives);

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
                           game->current_level->bg.r,
                           game->current_level->bg.g,
                           game->current_level->bg.b,
                           game->current_level->bg.a);
    SDL_RenderClear(renderer);

    // render game objects
    render_ball(renderer, game->ball);
    render_paddle(renderer, game->player);
    render_score_box(renderer, game->score, game->high_score, game->current_score, game->lives);

    // render bricks
    list_for_each(game->current_level->brick_list, render_brick);

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
    double speed_x = BASE_BALL_SPEED * game->current_level->difficulty_modifier;
    double speed_y = BASE_BALL_SPEED * game->current_level->difficulty_modifier;
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
            game->lives -= 1;
            Mix_Chunk *chunk;
            if (game->lives == 0) {
                chunk = game->sounds->game_over;
            } else {
                chunk = game->sounds->life_lost;
                reset_ball(game->ball);
                reset_paddle(game->player);
            }
            play_sound_effect(chunk);
        }
    }

    // check brick collisions
    list_for_each_with_param(game->current_level->brick_list,
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
            paddle->x += BASE_PADDLE_SPEED;
        } else {
            paddle->x = SCREEN_WIDTH - PADDLE_WIDTH;
        }
    }

    if (left_down) {
        if (paddle->x >= 0) {
            paddle->x -= BASE_PADDLE_SPEED;
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
close(void) {
    Mix_Quit();
    SDL_Quit();
}

internal void
toggle_menu_item(char **menu_item, char *option_one, char *option_two) {
    if (!strcmp(*menu_item, option_one)) {
        strcpy(*menu_item, option_two);
    } else {
        strcpy(*menu_item, option_one);
    }
}

internal int8_t
new_level_callback(SDL_Renderer *renderer, int menu_index, char **menu_item, void *param) {
    breaker_game *game = param;
    switch (menu_index) {
        case 0:
            reset_game(game, renderer, game->level_index, true);
            return false;
        case 1:
            toggle_menu_item(menu_item, "Disable Music", "Enable Music");
            game->score->music_on = !game->score->music_on;
            return true;
        case 2:
            toggle_menu_item(menu_item, "Disable Sound", "Enable Sound");
            game->score->sound_on = !game->score->sound_on;
            game->ball->sound_effects_on = game->score->sound_on;
            return true;
        case 3:
            save_game_state(game);
            close();
            exit(EXIT_SUCCESS);
        default:
            return true;
    }
}

internal int8_t
starting_menu_callback(SDL_Renderer *renderer, int menu_index, char **menu_item, void *param) {
    breaker_game *game = param;
    switch (menu_index) {
        case 0:
            reset_game(game, renderer, game->level_index, false);
            return false;
        case 1:
            toggle_menu_item(menu_item, "Disable Music", "Enable Music");
            game->score->music_on = !game->score->music_on;
            return true;
        case 2:
            toggle_menu_item(menu_item, "Disable Sound", "Enable Sound");
            game->score->sound_on = !game->score->sound_on;
            game->ball->sound_effects_on = game->score->sound_on;
            return true;
        case 3:
            save_game_state(game);
            close();
            exit(EXIT_SUCCESS);
        default:
            return true;
    }
}

internal int8_t
paused_menu_callback(SDL_Renderer *renderer, int menu_index, char **menu_item, void *param) {
    breaker_game *game = param;
    switch (menu_index) {
        case 0:
            return false;
        case 1:
            reset_game(game, renderer, STARTING_LEVEL, false);
            return false;
        case 2:
            toggle_menu_item(menu_item, "Disable Music", "Enable Music");
            game->score->music_on = !game->score->music_on;
            return true;
        case 3:
            toggle_menu_item(menu_item, "Disable Sound", "Enable Sound");
            game->score->sound_on = !game->score->sound_on;
            game->ball->sound_effects_on = game->score->sound_on;
            return true;
        case 4:
            save_game_state(game);
            close();
            exit(EXIT_SUCCESS);
        default:
            return true;
    }
}

internal void
display_breaker_menu(SDL_Renderer *renderer,
                     breaker_game *game,
                     int8_t paused,
                     int8_t new_level,
                     char *title, callback_function callback) {

    char *menu_items[5];
    int i = 0;
    if (paused || new_level) {
        menu_items[i++] = "Continue...";
    }
    if (!new_level) {
        menu_items[i++] = "New Game";
    }
    char music_menu_item[MENU_ITEM_SIZE] = {};
    char sound_menu_item[MENU_ITEM_SIZE] = {};
    strcpy(music_menu_item, game->score->music_on ? "Disable Music" : "Enable Music");
    strcpy(sound_menu_item, game->score->sound_on ? "Disable Sound" : "Enable Sound");
    menu_items[i++] = music_menu_item;
    menu_items[i++] = sound_menu_item;
    char quit[MENU_ITEM_SIZE] = "Quit";
    menu_items[i++] = quit;

    SDL_Rect bounds = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    menu starting_menu = init_menu(i, callback, menu_items, &SCREEN, &BLACK, &bounds);
    int8_t result = display_menu(renderer, starting_menu, SCORE_FONT, title, game);
    destroy_menu(starting_menu);
    if (result == QUIT_FROM_MENU) {
        close();
        exit(EXIT_SUCCESS);
    }
}

internal void
process_event(SDL_Event *event, breaker_game *game, int8_t *running) {
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
                case SDLK_SPACE:
                case SDLK_ESCAPE:
                case SDLK_PAUSE: {
                    game->key_right_down = false;
                    game->key_left_down = false;
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    if (game->score->music_on) {
                        pause_music();
                    }
                    display_breaker_menu(renderer, game, true, false, "Game Paused", paused_menu_callback);
                    if (game->score->music_on && Mix_PausedMusic()) {
                        pause_music();
                    }
                } break;
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
check_game_over(SDL_Renderer *renderer, breaker_game *game) {
    if (game->lives <= 0) {
        game->key_right_down = false;
        game->key_left_down = false;
        if (game->score->music_on) {
            pause_music();
        }
        game->level_index = STARTING_LEVEL;
        display_breaker_menu(renderer, game, false, false, "Game Over!", starting_menu_callback);
    } else if (game->current_level->bricks_left <= 0) {
        //TODO change this to increment level
        game->key_right_down = false;
        game->key_left_down = false;
        if (game->score->music_on) {
            pause_music();
        }
        game->level_index++;
        game->level_index %= MAX_LEVELS;
        display_breaker_menu(renderer, game, false, true, "New Level!", new_level_callback);
    }
}

internal void
init_level_music(Mix_Music **track_list) {
    track_list[0] = Mix_LoadMUS(LEVEL_1_TRACK);
    track_list[1] = Mix_LoadMUS(LEVEL_2_TRACK);
    track_list[2] = Mix_LoadMUS(LEVEL_3_TRACK);
    track_list[3] = Mix_LoadMUS(LEVEL_4_TRACK);
}

internal void
free_level_music(Mix_Music **track_list) {
    Mix_FreeMusic(track_list[0]);
    Mix_FreeMusic(track_list[1]);
    Mix_FreeMusic(track_list[2]);
    Mix_FreeMusic(track_list[3]);
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

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        error(SDL_GetError);
    }

    Mix_Chunk *wall_bounce = Mix_LoadWAV(WALL_BOUNCE);
    Mix_Chunk *life_lost = Mix_LoadWAV(LIFE_LOST);
    Mix_Chunk *game_over = Mix_LoadWAV(GAME_OVER);

    if (!wall_bounce || !life_lost || !game_over) {
        error(Mix_GetError);
    }

    breaker_sounds sounds = {
            wall_bounce,
            life_lost,
            game_over
    };

    breaker_ball ball = {
            SCREEN_WIDTH >> 1,
            SCREEN_HEIGHT >> 1,
            BALL_SIZE,
            random_bool() ? 1 : -1,
            1,
            true
    };

    breaker_paddle paddle = {
            (SCREEN_WIDTH >> 1) - (PADDLE_WIDTH >> 1),
            SCREEN_HEIGHT * .9f,
            PADDLE_WIDTH,
            PADDLE_HEIGHT,
    };

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

    level *current_level = NULL;

    char *level_files[MAX_LEVELS];
    level_files[0] = LEVEL_ONE_MAP;
    level_files[1] = LEVEL_TWO_MAP;
    level_files[2] = LEVEL_THREE_MAP;
    level_files[3] = LEVEL_FOUR_MAP;

    Mix_Music *music_tracks[MAX_LEVELS];
    init_level_music(music_tracks);

    breaker_game game = {
            &ball,
            &paddle,
            &box,
            &sounds,
            false,
            false,
            STARTING_LIVES,
            STARTING_SCORE,
            STARTING_SCORE,
            false,
            &mouse_loc,
            current_level,
            level_files,
            music_tracks,
            STARTING_LEVEL
    };

    // load previous preferences
    load_game_state(&game);

    // display menu
    display_breaker_menu(renderer, &game, false, false, "Breaker!", starting_menu_callback);

    int8_t running = true;
    SDL_Event event;
    uint32_t then;
    uint32_t now = SDL_GetTicks();

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
            ticks_passed = 0.0;
        }
        render(renderer, &game);
        check_game_over(renderer, &game);

#if DEBUG
        performance_profiling(per_count_frequency, &last_counter, &last_cycle_count);
#endif
    }

    //free resources
    //TODO free more stuff (font textures, etc.)
    free_level(game.current_level);
    free_level_music(game.music_files);
    free(music_c.bounds);
    free(sound_c.bounds);
    free(high_score_c.bounds);
    free(current_score_c.bounds);
    free(lives_c.bounds);
    Mix_FreeChunk(wall_bounce);
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

int
main(int argc, char** argv) {
    init();
    run();
    close();
    return EXIT_SUCCESS;
}
