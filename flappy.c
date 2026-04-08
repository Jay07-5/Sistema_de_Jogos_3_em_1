#include <stdlib.h>

#include "audio.h"
#include "effects.h"
#include "oled.h"
#include "game.h"

static void update_pipe(pipe_t *pipe) {
    pipe->x -= 3;
    if (pipe->x < -FLAPPY_PIPE_WIDTH) {
        pipe->x = WIDTH + 18;
        pipe->gap_y = 12 + (rand() % 24);
        pipe->counted = false;
    }
}

static bool flappy_collides_with_pipe(const game_context_t *ctx, const pipe_t *pipe) {
    const int bird_x = 20;
    bool overlaps_x = bird_x + FLAPPY_BIRD_SIZE >= pipe->x && bird_x <= pipe->x + FLAPPY_PIPE_WIDTH;
    bool hits_top = ctx->bird_y <= pipe->gap_y;
    bool hits_bottom = ctx->bird_y + FLAPPY_BIRD_SIZE >= pipe->gap_y + FLAPPY_GAP;

    return overlaps_x && (hits_top || hits_bottom);
}

void reset_flappy(game_context_t *ctx) {
    ctx->bird_y = 28;
    ctx->bird_velocity = 0;
    ctx->flappy_score = 0;
    ctx->flappy_game_over = false;
    ctx->flappy_game_over_sound_played = false;

    for (int i = 0; i < FLAPPY_PIPE_COUNT; i++) {
        ctx->pipes[i].x = WIDTH + (i * 42);
        ctx->pipes[i].gap_y = 14 + (rand() % 22);
        ctx->pipes[i].counted = false;
    }
}

void flappy_step(game_context_t *ctx) {
    if (ctx->flappy_game_over) {
        if (!ctx->flappy_game_over_sound_played) {
            audio_play_flappy_die();
            ctx->flappy_game_over_sound_played = true;
        }
        oled_clear();
        draw_frame();
        draw_header("BIRD", ctx->flappy_score, ctx->best_flappy_score);
        draw_game_over_overlay("BATEU!", "A: novo B: menu");
        oled_update();

        if (button_pressed()) {
            reset_flappy(ctx);
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

    if (button_pressed() || joystick_axis_value(joystick_y_raw()) == -1) {
        ctx->bird_velocity = -4;
        audio_play_flappy_jump();
    }

    ctx->bird_velocity += 1;
    if (ctx->bird_velocity > 3) {
        ctx->bird_velocity = 3;
    }
    ctx->bird_y += ctx->bird_velocity;

    if (ctx->bird_y < 12 || ctx->bird_y > HEIGHT - FLAPPY_BIRD_SIZE - 1) {
        ctx->flappy_game_over = true;
        update_best_score(&ctx->best_flappy_score, ctx->flappy_score);
        effects_trigger_game_over();
    }

    for (int i = 0; i < FLAPPY_PIPE_COUNT; i++) {
        update_pipe(&ctx->pipes[i]);

        if (!ctx->pipes[i].counted && ctx->pipes[i].x + FLAPPY_PIPE_WIDTH < 20) {
            ctx->pipes[i].counted = true;
            ctx->flappy_score++;
            update_best_score(&ctx->best_flappy_score, ctx->flappy_score);
            audio_play_flappy_point();
        }

        if (!ctx->flappy_game_over && flappy_collides_with_pipe(ctx, &ctx->pipes[i])) {
            ctx->flappy_game_over = true;
            update_best_score(&ctx->best_flappy_score, ctx->flappy_score);
            effects_trigger_game_over();
        }
    }

    oled_clear();
    draw_frame();
    draw_header("BIRD", ctx->flappy_score, ctx->best_flappy_score);
    oled_rect(4, HEIGHT - 3, WIDTH - 8, 1, 1);

    for (int i = 0; i < FLAPPY_PIPE_COUNT; i++) {
        oled_rect(ctx->pipes[i].x, 12, FLAPPY_PIPE_WIDTH, ctx->pipes[i].gap_y - 12, 1);
        oled_rect(ctx->pipes[i].x, ctx->pipes[i].gap_y + FLAPPY_GAP, FLAPPY_PIPE_WIDTH,
                  HEIGHT - (ctx->pipes[i].gap_y + FLAPPY_GAP) - 2, 1);
    }

    oled_rect(20, ctx->bird_y, FLAPPY_BIRD_SIZE, FLAPPY_BIRD_SIZE, 1);
    oled_rect(25, ctx->bird_y + 2, 2, 2, 1);
    oled_update();
    sleep_ms(55);
}
