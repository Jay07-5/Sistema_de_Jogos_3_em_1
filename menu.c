#include <stdlib.h>

#include "audio.h"
#include "oled.h"
#include "game.h"
#include "web.h"

// menu.c desenha e processa o menu principal do jogo.
// O menu permite escolher entre Cobra, Pong, Bird e sair.

static void draw_menu_card(int y, const char *label, bool selected, int icon_width) {
    if (selected) {
        // Desenha o item selecionado com fundo preenchido.
        oled_rect(6, y, 116, 10, 1);
        oled_rect(10, y + 2, 4, 6, 0);
        oled_rect(18, y + 2, icon_width, 5, 0);
        oled_text(36, y + 2, label, 0);
    } else {
        oled_rect(6, y, 116, 1, 1);
        oled_rect(6, y + 9, 116, 1, 1);
        oled_rect(6, y, 1, 10, 1);
        oled_rect(121, y, 1, 10, 1);
        oled_rect(18, y + 2, icon_width, 5, 1);
        oled_text(36, y + 2, label, 1);
    }
}

void draw_menu(game_context_t *ctx) {
    // Lê o joystick vertical e move a seleção.
    int joy_y = joystick_axis_value(joystick_y_raw());

    if (!ctx->menu_axis_lock && joy_y != 0) {
        ctx->menu_option += joy_y;
        if (ctx->menu_option < 0) {
            ctx->menu_option = MENU_OPTIONS - 1;
        }
        if (ctx->menu_option >= MENU_OPTIONS) {
            ctx->menu_option = 0;
        }
        ctx->menu_axis_lock = true;
    } else if (joy_y == 0) {
        // Solta o eixo para permitir nova mudança de opção.
        ctx->menu_axis_lock = false;
    }

    oled_clear();
    draw_frame();

    oled_rect(6, 4, 116, 12, 1);
    oled_text(27, 7, "ARCADE 2040", 0);

    oled_text(8, 20, "Y move", 1);
    oled_text(66, 20, "A abre", 1);
    oled_text(66, 27, "B volta", 1);
    // Exibe o status e o IP da conexão Wi-Fi no OLED.
    // Essas strings são atualizadas pelo módulo web em web_poll().
    oled_text(8, 44, web_status_string(), 1);
    oled_text(8, 52, web_ip_string(), 1);

    draw_menu_card(28, menu_options[0], ctx->menu_option == 0, 10);
    draw_menu_card(39, menu_options[1], ctx->menu_option == 1, 10);
    draw_menu_card(50, menu_options[2], ctx->menu_option == 2, 10);

    if (ctx->menu_option == 3) {
        oled_rect(84, 50, 32, 10, 1);
        oled_text(90, 52, "SAIR", 0);
    } else {
        oled_rect(84, 50, 32, 1, 1);
        oled_rect(84, 59, 32, 1, 1);
        oled_rect(84, 50, 1, 10, 1);
        oled_rect(115, 50, 1, 10, 1);
        oled_text(90, 52, "SAIR", 1);
    }

    oled_update();

    if (button_pressed()) {
        // Seleção do menu: entra no estado escolhido, ou sai do jogo.
        audio_play_select();
        if (ctx->menu_option == 0) {
            enter_state(ctx, STATE_SNAKE);
        } else if (ctx->menu_option == 1) {
            enter_state(ctx, STATE_PONG);
        } else if (ctx->menu_option == 2) {
            enter_state(ctx, STATE_FLAPPY);
        } else {
            oled_clear();
            draw_frame();
            oled_text(22, 22, "ATE A PROXIMA", 1);
            oled_text(24, 34, "REINICIE A PICO", 1);
            oled_update();
            sleep_ms(500);
        }
    }
}
