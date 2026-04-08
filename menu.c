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
    oled_text(16, 7, "SISTEMA DE JOGOS", 0);

    oled_text(8, 18, "Y: move", 1);
    oled_text(56, 18, "A: abre", 1);
    oled_text(8, 26, web_status_string(), 1);

    draw_menu_card(34, menu_options[0], ctx->menu_option == 0, 10);
    draw_menu_card(44, menu_options[1], ctx->menu_option == 1, 10);
    draw_menu_card(54, menu_options[2], ctx->menu_option == 2, 10);
    draw_menu_card(64, menu_options[3], ctx->menu_option == 3, 10);

    if (ctx->menu_option == 4) {
        oled_rect(84, 54, 32, 10, 1);
        oled_text(90, 56, "SAIR", 0);
    } else {
        oled_rect(84, 54, 32, 1, 1);
        oled_rect(84, 63, 32, 1, 1);
        oled_rect(84, 54, 1, 10, 1);
        oled_rect(115, 54, 1, 10, 1);
        oled_text(90, 56, "SAIR", 1);
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
