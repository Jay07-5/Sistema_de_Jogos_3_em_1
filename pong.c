#include <stdlib.h>

#include "audio.h"
#include "effects.h"
#include "oled.h"
#include "game.h"

void reset_pong(game_context_t *ctx) {
    ctx->pong_player_y = 24;
    ctx->pong_ai_y = 24;
    ctx->pong_ball_x = WIDTH / 2;
    ctx->pong_ball_y = (HEIGHT + 12) / 2;
    ctx->pong_ball_vx = (rand() & 1) ? 4 : -4;
    ctx->pong_ball_vy = (rand() % 7) - 3;
    if (ctx->pong_ball_vy == 0) {
        ctx->pong_ball_vy = 1;
    }
    ctx->pong_player_score = 0;
    ctx->pong_ai_score = 0;
    ctx->pong_game_over = false;
    ctx->pong_player_won = false;
    ctx->pong_game_over_sound_played = false;
}

void pong_step(game_context_t *ctx) {
    if (ctx->pong_game_over) {
        if (!ctx->pong_game_over_sound_played) {
            audio_play_game_over();
            ctx->pong_game_over_sound_played = true;
        }
        oled_clear();
        draw_frame();
        draw_header("PONG", ctx->pong_player_score, ctx->best_pong_score);
        if (ctx->pong_player_won) {
            draw_game_over_overlay("VOCE VENCEU", "A: de novo");
        } else {
            draw_game_over_overlay("VOCE PERDEU", "A: retry");
        }
        oled_text(24, 44, "B: MENU", 1);
        oled_update();

        if (button_pressed()) {
            reset_pong(ctx);
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

    ctx->pong_player_y += joystick_axis_value(joystick_y_raw()) * 3;
    ctx->pong_player_y = clamp_int(ctx->pong_player_y, 12, HEIGHT - PONG_PADDLE_H - 2);

    if (ctx->pong_ball_y + (PONG_BALL_SIZE / 2) > ctx->pong_ai_y + (PONG_PADDLE_H / 2)) {
        ctx->pong_ai_y += 2;
    } else if (ctx->pong_ball_y + (PONG_BALL_SIZE / 2) < ctx->pong_ai_y + (PONG_PADDLE_H / 2)) {
        ctx->pong_ai_y -= 2;
    }
    ctx->pong_ai_y = clamp_int(ctx->pong_ai_y, 12, HEIGHT - PONG_PADDLE_H - 2);

    ctx->pong_ball_x += ctx->pong_ball_vx;
    ctx->pong_ball_y += ctx->pong_ball_vy;

    if (ctx->pong_ball_y <= 12) {
        ctx->pong_ball_vy = -ctx->pong_ball_vy;
        ctx->pong_ball_y = 12;
        audio_play_pong_wall();
    } else if (ctx->pong_ball_y >= HEIGHT - PONG_BALL_SIZE) {
        ctx->pong_ball_vy = -ctx->pong_ball_vy;
        ctx->pong_ball_y = HEIGHT - PONG_BALL_SIZE;
        audio_play_pong_wall();
    }

    if (ctx->pong_ball_x <= 5 + PONG_PADDLE_W &&
        ctx->pong_ball_y + PONG_BALL_SIZE >= ctx->pong_player_y &&
        ctx->pong_ball_y <= ctx->pong_player_y + PONG_PADDLE_H) {
        ctx->pong_ball_x = 5 + PONG_PADDLE_W;
        ctx->pong_ball_vx = 4;
        ctx->pong_ball_vy = ((ctx->pong_ball_y - ctx->pong_player_y) / 3) - 2;
        if (ctx->pong_ball_vy == 0) {
            ctx->pong_ball_vy = 1;
        }
        audio_play_pong_paddle();
    }

    if (ctx->pong_ball_x + PONG_BALL_SIZE >= WIDTH - 6 - PONG_PADDLE_W &&
        ctx->pong_ball_y + PONG_BALL_SIZE >= ctx->pong_ai_y &&
        ctx->pong_ball_y <= ctx->pong_ai_y + PONG_PADDLE_H) {
        ctx->pong_ball_x = WIDTH - 6 - PONG_PADDLE_W - PONG_BALL_SIZE;
        ctx->pong_ball_vx = -4;
        ctx->pong_ball_vy = ((ctx->pong_ball_y - ctx->pong_ai_y) / 3) - 2;
        if (ctx->pong_ball_vy == 0) {
            ctx->pong_ball_vy = -1;
        }
        audio_play_pong_paddle();
    }

    if (ctx->pong_ball_x < 0) {
        ctx->pong_ai_score++;
        if (ctx->pong_ai_score >= PONG_SCORE_LIMIT) {
            ctx->pong_game_over = true;
            ctx->pong_player_won = false;
            update_best_score(&ctx->best_pong_score, ctx->pong_player_score);
            effects_trigger_game_over();
        } else {
            ctx->pong_ball_x = WIDTH / 2;
            ctx->pong_ball_y = 30;
            ctx->pong_ball_vx = 4;
            ctx->pong_ball_vy = (rand() % 7) - 3;
            if (ctx->pong_ball_vy == 0) {
                ctx->pong_ball_vy = 1;
            }
            audio_play_point();
        }
    }

    if (ctx->pong_ball_x > WIDTH) {
        ctx->pong_player_score++;
        if (ctx->pong_player_score >= PONG_SCORE_LIMIT) {
            ctx->pong_game_over = true;
            ctx->pong_player_won = true;
            update_best_score(&ctx->best_pong_score, ctx->pong_player_score);
            effects_trigger_game_over();
        } else {
            ctx->pong_ball_x = WIDTH / 2;
            ctx->pong_ball_y = 30;
            ctx->pong_ball_vx = -4;
            ctx->pong_ball_vy = (rand() % 7) - 3;
            if (ctx->pong_ball_vy == 0) {
                ctx->pong_ball_vy = -1;
            }
            audio_play_point();
        }
    }

    oled_clear();
    draw_frame();
    draw_header("PONG", ctx->pong_player_score, ctx->best_pong_score);
    oled_text(48, 54, "X", 1);
    oled_text(58, 54, "IA", 1);
    oled_text(35, 54, "P", 1);

    oled_rect(5, ctx->pong_player_y, PONG_PADDLE_W, PONG_PADDLE_H, 1);
    oled_rect(WIDTH - 6 - PONG_PADDLE_W, ctx->pong_ai_y, PONG_PADDLE_W, PONG_PADDLE_H, 1);
    oled_rect(ctx->pong_ball_x, ctx->pong_ball_y, PONG_BALL_SIZE, PONG_BALL_SIZE, 1);
    oled_update();
    sleep_ms(35);
}
