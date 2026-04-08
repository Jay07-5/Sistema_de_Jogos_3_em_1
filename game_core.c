#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "hardware/adc.h"
#include "oled.h"
#include "game.h"

// game_core.c contém utilitários compartilhados pelo jogo,
// incluindo leitura de joystick, estado do menu, desenho de UI e transição de estados.
// Este módulo não implementa nenhum jogo específico, apenas serviços comuns.

const char *menu_options[MENU_OPTIONS] = {
    "Cobra",
    "Pong",
    "Bird",
    "WiFi",
    "Sair"
};

void game_init_context(game_context_t *ctx) {
    // Inicializa o contexto do jogo com valores padrão.
    // Define o menu como estado inicial e zera o melhor placar.
    ctx->estado_atual = STATE_MENU;
    ctx->menu_option = 0;
    ctx->menu_axis_lock = false;

    ctx->best_snake_score = 0;
    ctx->best_pong_score = 0;
    ctx->best_flappy_score = 0;

    reset_snake(ctx);
    reset_pong(ctx);
    reset_flappy(ctx);
    ctx->estado_atual = STATE_MENU;
}

// Leitura genérica de ADC com pequeno atraso para estabilização.
uint16_t read_adc_channel(unsigned int input) {
    adc_select_input(input);
    sleep_us(8);
    return adc_read();
}

int joystick_x_raw(void) {
    return (int)read_adc_channel(JOYSTICK_Y_ADC);
}

int joystick_y_raw(void) {
    return 4095 - (int)read_adc_channel(JOYSTICK_X_ADC);
}

int joystick_axis_value(int raw) {
    // Converte a leitura analógica do joystick em -1, 0 ou 1.
    // O zero é usado quando o joystick está dentro da zona morta.
    if (raw < JOYSTICK_DEADZONE_LOW) {
        return -1;
    }
    if (raw > JOYSTICK_DEADZONE_HIGH) {
        return 1;
    }
    return 0;
}

// Debounce simples para os botões físicos.
static bool gpio_button_pressed(uint pin) {
    if (gpio_get(pin) == 0) {
        sleep_ms(20);
        if (gpio_get(pin) == 0) {
            while (gpio_get(pin) == 0) {
                sleep_ms(10);
            }
            sleep_ms(20);
            return true;
        }
    }
    return false;
}

bool joystick_button_pressed(void) {
    return gpio_button_pressed(JOYSTICK_BUTTON_PIN);
}

bool button_pressed(void) {
    if (joystick_button_pressed()) {
        return true;
    }
    return gpio_button_pressed(BUTTON_A_PIN);
}

bool button_a_pressed(void) {
    return gpio_button_pressed(BUTTON_A_PIN);
}

bool button_b_pressed(void) {
    return gpio_button_pressed(BUTTON_B_PIN);
}

bool back_pressed(void) {
    // O botão 'back' é tratado como o botão B no projeto.
    return button_b_pressed();
}

// Limita um valor inteiro a um intervalo mínimo/máximo.
int clamp_int(int value, int min_value, int max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

void draw_frame(void) {
    // Desenha a borda do campo de jogo no OLED.
    oled_rect(0, 0, WIDTH, 1, 1);
    oled_rect(0, HEIGHT - 1, WIDTH, 1, 1);
    oled_rect(0, 0, 1, HEIGHT, 1);
    oled_rect(WIDTH - 1, 0, 1, HEIGHT, 1);
}

// Desenha o cabeçalho comum presente em todos os jogos.
void draw_header(const char *title, int score, int best) {
    char score_text[16];
    char best_text[16];

    snprintf(score_text, sizeof(score_text), "P:%d", score);
    snprintf(best_text, sizeof(best_text), "R:%d", best);

    oled_rect(0, 0, WIDTH, 10, 1);
    oled_text(2, 1, title, 0);
    oled_text(58, 1, score_text, 0);
    oled_text(92, 1, best_text, 0);
}

void draw_game_over_overlay(const char *line1, const char *line2) {
    // Desenha a caixa de fim de jogo com duas linhas de texto.
    oled_rect(14, 20, 100, 22, 1);
    oled_rect(16, 22, 96, 18, 0);
    oled_text(24, 24, line1, 1);
    oled_text(18, 32, line2, 1);
}

// Atualiza o melhor placar se a pontuação atual for maior.
void update_best_score(int *best_score, int score) {
    if (score > *best_score) {
        *best_score = score;
    }
}

void enter_state(game_context_t *ctx, game_state_t next_state) {
    // Altera o estado atual do jogo e reinicia o modo selecionado.
    ctx->estado_atual = next_state;

    if (next_state == STATE_SNAKE) {
        reset_snake(ctx);
    } else if (next_state == STATE_PONG) {
        reset_pong(ctx);
    } else if (next_state == STATE_FLAPPY) {
        reset_flappy(ctx);
    }
}
