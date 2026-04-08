#pragma once

#include <stdbool.h>

void audio_init(void);
void audio_set_menu_active(bool active);
void audio_update(void);
void audio_play_select(void);
void audio_play_point(void);
void audio_play_game_over(void);
void audio_play_pong_wall(void);
void audio_play_pong_paddle(void);
void audio_play_snake_eat(void);
void audio_play_snake_die(void);
void audio_play_snake_move(void);
void audio_play_flappy_jump(void);
void audio_play_flappy_point(void);
void audio_play_flappy_die(void);
