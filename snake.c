#include <stdlib.h>

#include "audio.h"
#include "effects.h"
#include "oled.h"
#include "game.h"

// snake.c implementa o jogo da cobrinha.
// Controla a movimentação, colisões, comida e desenho da cobra.

#define SNAKE_COLS (WIDTH / SNAKE_CELL)
#define SNAKE_ROWS ((HEIGHT - SNAKE_BOARD_TOP) / SNAKE_CELL)

static void reset_snake_food(game_context_t *ctx) {
    // Gera uma posição aleatória para a comida, evitando sobreposição com a cobra.
    bool valid_position = false;

    while (!valid_position) {
        valid_position = true;
        ctx->snake_food_x = rand() % SNAKE_COLS;
        ctx->snake_food_y = rand() % SNAKE_ROWS;

        for (int i = 0; i < ctx->snake_size; i++) {
            if (ctx->snake_x[i] == ctx->snake_food_x && ctx->snake_y[i] == ctx->snake_food_y) {
                valid_position = false;
                break;
            }
        }
    }
}

void reset_snake(game_context_t *ctx) {
    // Inicializa a cobra no centro do campo e zera as variáveis de jogo.
    ctx->snake_size = 4;
    ctx->snake_dir_x = 1;
    ctx->snake_dir_y = 0;
    ctx->snake_score = 0;
    ctx->snake_game_over = false;
    ctx->snake_game_over_sound_played = false;

    for (int i = 0; i < ctx->snake_size; i++) {
        ctx->snake_x[i] = 8 - i;
        ctx->snake_y[i] = 6;
    }

    reset_snake_food(ctx);
}

static void snake_handle_input(game_context_t *ctx) {
    int joy_x = joystick_axis_value(joystick_x_raw());
    int joy_y = joystick_axis_value(joystick_y_raw());

    // Atualiza a direção da cobra evitando reversão imediata.
    if (joy_x == -1 && ctx->snake_dir_x != 1) {
        ctx->snake_dir_x = -1;
        ctx->snake_dir_y = 0;
    } else if (joy_x == 1 && ctx->snake_dir_x != -1) {
        ctx->snake_dir_x = 1;
        ctx->snake_dir_y = 0;
    } else if (joy_y == -1 && ctx->snake_dir_y != 1) {
        ctx->snake_dir_x = 0;
        ctx->snake_dir_y = -1;
    } else if (joy_y == 1 && ctx->snake_dir_y != -1) {
        ctx->snake_dir_x = 0;
        ctx->snake_dir_y = 1;
    }
}

void snake_step(game_context_t *ctx) {
    // Atualiza um passo do jogo Snake a cada frame.
    if (ctx->snake_game_over) {
        if (!ctx->snake_game_over_sound_played) {
            audio_play_game_over();
            ctx->snake_game_over_sound_played = true;
        }
        oled_clear();
        draw_frame();
        draw_header("SNAKE", ctx->snake_score, ctx->best_snake_score);
        draw_game_over_overlay("GAME OVER", "A: novo B: menu");
        oled_update();

        if (button_pressed()) {
            reset_snake(ctx);
        } else if (back_pressed()) {
            ctx->estado_atual = STATE_MENU;
        }
        sleep_ms(80);
        return;
    }

    if (back_pressed()) {
        ctx->estado_atual = STATE_MENU;
        return;
    }

    // Processa controle e toca som de movimento.
    snake_handle_input(ctx);
    audio_play_snake_move();

    // Move cada segmento para posição do anterior e avança a cabeça.
    for (int i = ctx->snake_size; i > 0; i--) {
        ctx->snake_x[i] = ctx->snake_x[i - 1];
        ctx->snake_y[i] = ctx->snake_y[i - 1];
    }

    ctx->snake_x[0] += ctx->snake_dir_x;
    ctx->snake_y[0] += ctx->snake_dir_y;

    // Verifica colisão com as paredes.
    if (ctx->snake_x[0] < 0 || ctx->snake_x[0] >= SNAKE_COLS || ctx->snake_y[0] < 0 || ctx->snake_y[0] >= SNAKE_ROWS) {
        ctx->snake_game_over = true;
        update_best_score(&ctx->best_snake_score, ctx->snake_score);
        effects_trigger_game_over();
    }

    // Verifica colisão com o próprio corpo.
    for (int i = 1; i < ctx->snake_size && !ctx->snake_game_over; i++) {
        if (ctx->snake_x[0] == ctx->snake_x[i] && ctx->snake_y[0] == ctx->snake_y[i]) {
            ctx->snake_game_over = true;
            update_best_score(&ctx->best_snake_score, ctx->snake_score);
            effects_trigger_game_over();
        }
    }

    // Verifica se comeu a comida e cresce.
    if (!ctx->snake_game_over && ctx->snake_x[0] == ctx->snake_food_x && ctx->snake_y[0] == ctx->snake_food_y) {
        ctx->snake_score++;
        if (ctx->snake_size < SNAKE_MAX_SIZE - 1) {
            ctx->snake_size++;
        }
        update_best_score(&ctx->best_snake_score, ctx->snake_score);
        reset_snake_food(ctx);
        audio_play_snake_eat();
    }

    // Desenha o campo, a pontuação, a comida e a cobra.
    oled_clear();
    draw_frame();
    draw_header("SNAKE", ctx->snake_score, ctx->best_snake_score);

    oled_rect(ctx->snake_food_x * SNAKE_CELL, SNAKE_BOARD_TOP + ctx->snake_food_y * SNAKE_CELL, 3, 3, 1);
    for (int i = 0; i < ctx->snake_size; i++) {
        int px = ctx->snake_x[i] * SNAKE_CELL;
        int py = SNAKE_BOARD_TOP + ctx->snake_y[i] * SNAKE_CELL;
        oled_rect(px, py, SNAKE_CELL - 1, SNAKE_CELL - 1, 1);
    }

    oled_update();
    sleep_ms(130);
}
