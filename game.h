#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "pico/stdlib.h"

#define JOYSTICK_X_ADC 0
#define JOYSTICK_Y_ADC 1
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_BUTTON_PIN 22
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

#define MENU_OPTIONS 4
#define JOYSTICK_DEADZONE_LOW 1400
#define JOYSTICK_DEADZONE_HIGH 2700

#define SNAKE_CELL 4
#define SNAKE_BOARD_TOP 12
#define SNAKE_MAX_SIZE 80

#define PONG_PADDLE_H 14
#define PONG_PADDLE_W 3
#define PONG_BALL_SIZE 3
#define PONG_SCORE_LIMIT 9

#define FLAPPY_BIRD_SIZE 5
#define FLAPPY_PIPE_WIDTH 10
#define FLAPPY_GAP 18
#define FLAPPY_PIPE_COUNT 3

typedef enum {
    STATE_MENU = 0,
    STATE_SNAKE,
    STATE_PONG,
    STATE_FLAPPY,
} game_state_t;

typedef struct {
    int x;
    int gap_y;
    bool counted;
} pipe_t;

typedef struct {
    game_state_t estado_atual;
    int menu_option;
    bool menu_axis_lock;

    int best_snake_score;
    int best_pong_score;
    int best_flappy_score;

    int snake_x[SNAKE_MAX_SIZE];
    int snake_y[SNAKE_MAX_SIZE];
    int snake_size;
    int snake_dir_x;
    int snake_dir_y;
    int snake_food_x;
    int snake_food_y;
    int snake_score;
    bool snake_game_over;
    bool snake_game_over_sound_played;

    int pong_player_y;
    int pong_ai_y;
    int pong_ball_x;
    int pong_ball_y;
    int pong_ball_vx;
    int pong_ball_vy;
    int pong_player_score;
    int pong_ai_score;
    bool pong_game_over;
    bool pong_player_won;
    bool pong_game_over_sound_played;

    int bird_y;
    int bird_velocity;
    int flappy_score;
    bool flappy_game_over;
    bool flappy_game_over_sound_played;
    pipe_t pipes[FLAPPY_PIPE_COUNT];
} game_context_t;

extern const char *menu_options[MENU_OPTIONS];

void game_init_context(game_context_t *ctx);
void enter_state(game_context_t *ctx, game_state_t next_state);

uint16_t read_adc_channel(unsigned int input);
int joystick_x_raw(void);
int joystick_y_raw(void);
int joystick_axis_value(int raw);
bool joystick_button_pressed(void);
bool button_pressed(void);
bool button_a_pressed(void);
bool button_b_pressed(void);
bool back_pressed(void);
int clamp_int(int value, int min_value, int max_value);
void draw_frame(void);
void draw_header(const char *title, int score, int best);
void draw_game_over_overlay(const char *line1, const char *line2);
void update_best_score(int *best_score, int score);

void reset_snake(game_context_t *ctx);
void snake_step(game_context_t *ctx);

void reset_pong(game_context_t *ctx);
void pong_step(game_context_t *ctx);

void reset_flappy(game_context_t *ctx);
void flappy_step(game_context_t *ctx);

void draw_menu(game_context_t *ctx);
