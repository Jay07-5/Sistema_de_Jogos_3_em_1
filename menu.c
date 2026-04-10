#include <stdlib.h>

#include "audio.h"
#include "oled.h"
#include "game.h"
#include "web.h"

// menu.c desenha e processa o menu principal do jogo.
// O menu permite escolher entre Cobra, Pong, Bird e sair.

enum {
    MENU_CARD_X = 6,
    MENU_CARD_W = 116,
    MENU_CARD_H = 7,
    MENU_TEXT_X = 28
};

static void draw_menu_card(int y, const char *label, bool selected, int icon_width) {
    if (selected) {
        // Desenha o item selecionado com fundo preenchido.
        oled_rect(MENU_CARD_X, y, MENU_CARD_W, MENU_CARD_H, 1);
        oled_rect(10, y + 2, 4, 3, 0);
        oled_rect(18, y + 2, icon_width, 2, 0);
        oled_text(MENU_TEXT_X, y + 1, label, 0);
    } else {
        oled_rect(MENU_CARD_X, y, MENU_CARD_W, 1, 1);
        oled_rect(MENU_CARD_X, y + MENU_CARD_H - 1, MENU_CARD_W, 1, 1);
        oled_rect(MENU_CARD_X, y, 1, MENU_CARD_H, 1);
        oled_rect(MENU_CARD_X + MENU_CARD_W - 1, y, 1, MENU_CARD_H, 1);
        oled_rect(18, y + 2, icon_width, 2, 1);
        oled_text(MENU_TEXT_X, y + 1, label, 1);
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

    oled_rect(6, 3, 116, 10, 1);
    oled_text(16, 5, "SISTEMA DE JOGOS", 0);

    oled_text(8, 15, "Y move", 1);
    oled_text(54, 15, "A abre", 1);
    oled_text(8, 23, web_status_string(), 1);

    draw_menu_card(29, menu_options[0], ctx->menu_option == 0, 6);
    draw_menu_card(36, menu_options[1], ctx->menu_option == 1, 6);
    draw_menu_card(43, menu_options[2], ctx->menu_option == 2, 6);
    draw_menu_card(50, menu_options[3], ctx->menu_option == 3, 6);
    draw_menu_card(57, menu_options[4], ctx->menu_option == 4, 6);

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
        } else if (ctx->menu_option == 3) {
            // Exibe a tela de informações Wi-Fi / IP
            oled_clear();
            draw_frame();
            oled_rect(6, 4, 116, 12, 1);
            oled_text(20, 7, "CONEXAO WiFi", 0);
            oled_text(8, 20, web_status_string(), 1);
            oled_text(8, 28, web_ip_string(), 1);
            oled_text(8, 40, "Acesse pelo navegador", 1);
            oled_text(8, 48, "do seu celular", 1);
            oled_text(8, 56, "Pressione B para voltar", 1);
            oled_update();
            
            // Aguarda pressão de B para voltar ao menu
            while (!back_pressed()) {
                sleep_ms(50);
                web_poll(ctx);
            }
            while (back_pressed()) {
                sleep_ms(50);
            }
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
