//
// Created by bryan on 7/24/15.
//

#ifndef SDL_GRAPHICS_BREAKER_H
#define SDL_GRAPHICS_BREAKER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "list.h"

#define SCREEN_WIDTH 576
#define SCREEN_HEIGHT 850
#define AUDIO_FREQUENCY 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNK_SIZE 2048
#define BALL_SIZE 5
#define BASE_PADDLE_SPEED 5
#define BASE_BALL_SPEED 5
#define BASE_BRICK_VALUE 100
#define PADDLE_HEIGHT 10
#define PADDLE_WIDTH 75
#define BRICK_WIDTH SCREEN_WIDTH / 12
#define BRICK_HEIGHT 15
#define CEILING 75
#define BUTTON_SIZE 40
#define SCORE_OFFSET 5
#define BUTTON_DELAY 100
#define STARTING_LIVES 3
#define STARTING_SCORE 0
#define SCORE_PADDING 15


#define WALL_BOUNCE "resources/sounds/breaker/effects/wall_bounce.wav"
#define BRICK_BOUNCE "resources/sounds/breaker/effects/brick_bounce.wav"
#define BRICK_BREAK "resources/sounds/breaker/effects/brick_break.wav"
#define LIFE_LOST "resources/sounds/breaker/effects/life_lost.wav"
#define GAME_OVER "resources/sounds/breaker/effects/game_over.wav"
#define LEVEL_1_TRACK "resources/sounds/breaker/music/level_1.wav"
#define LEVEL_2_TRACK "resources/sounds/breaker/music/level_2.wav"
#define LEVEL_3_TRACK "resources/sounds/breaker/music/level_3.wav"
#define LEVEL_4_TRACK "resources/sounds/breaker/music/level_4.wav"
#define MUSIC_ON "resources/icons/music_on.png"
#define MUSIC_OFF "resources/icons/music_off.png"
#define SOUND_ON "resources/icons/sound_on.png"
#define SOUND_OFF "resources/icons/sound_off.png"
#define SCORE_TEXTURE "resources/textures/score_box.png"
#define SCORE_FONT "resources/fonts/built_tiling.ttf"
#define CONFIG_FILE_DIR "/.sdl"
#define CONFIG_FILE "/breaker.conf"

#define PREF_KEY_SCORE "HIGH_SCORE"
#define PREF_KEY_MUSIC "MUSIC_ON"
#define PREF_KEY_SOUND "SOUND_ON"

typedef enum BRICK_TYPE {
    NORMAL,
    DOUBLE,
    TRIPLE,
    UNBREAKABLE
} BRICK_TYPE;

typedef struct point {
    int x;
    int y;
} point;

typedef struct insets {
    int left;
    int right;
    int top;
    int bottom;
} insets;

typedef struct label {
    SDL_Texture *text;
    point location;
    int text_width;
    int text_height;
} label;

typedef struct component {
    label *component_label;
    SDL_Texture *icon;
    SDL_Texture *alt_icon;
    SDL_Rect *bounds;
    insets inset;
} component;

typedef struct score_box {
    component *music_button;
    component *sound_button;
    component *high_score_field;
    component *current_score_field;
    component *lives_field;
    int8_t music_on;
    int8_t sound_on;
    TTF_Font *text_font;
} score_box;

typedef struct breaker_sounds {
    Mix_Music *music;
    Mix_Chunk *wall_bounce;
    Mix_Chunk *life_lost;
    Mix_Chunk *game_over;
} breaker_sounds;

typedef struct breaker_brick {
    int x;
    int y;
    int width;
    int height;
    int value;
    int8_t visible;
    SDL_Color *base_color;
    SDL_Renderer *renderer;
    Mix_Chunk *brick_break;
    Mix_Chunk *brick_bounce;
    BRICK_TYPE type;
} breaker_brick;

typedef struct breaker_paddle {
    float x;
    float y;
    int width;
    int height;
} breaker_paddle;

typedef struct breaker_ball {
    float x;
    float y;
    int radius;
    int8_t x_dir;
    int8_t y_dir;
    int8_t sound_effects_on;
} breaker_ball;

typedef struct breaker_game {
    breaker_ball *ball;
    breaker_paddle *player;
    score_box *score;
    breaker_sounds *sounds;
    int8_t key_right_down;
    int8_t key_left_down;
    list *brick_list;
    int lives;
    int high_score;
    int current_score;
    int8_t mouse_down;
    point *mouse_loc;
} breaker_game;

#endif //SDL_GRAPHICS_BREAKER_H
