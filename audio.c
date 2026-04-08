#include "audio.h"

#include <stddef.h>
#include <stdint.h>

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

// audio.c controla a geração de sons no buzzer do Pico.
// Cada efeito de áudio é definido como uma sequência de notas e durações.

#define BUZZER_PIN 21
#define AUDIO_CLOCK_DIV 4.0f
#define AUDIO_DUTY_DIVISOR 48u

typedef struct {
    uint16_t freq;
    uint16_t duration_ms;
} note_t;

static const note_t select_sfx[] = {
    {440, 35},
    {523, 45},
    {0, 30}
};

static const note_t point_sfx[] = {
    {523, 35},
    {659, 50},
    {0, 30}
};

static const note_t game_over_sfx[] = {
    {784, 70},
    {698, 70},
    {659, 80},
    {587, 90},
    {523, 100},
    {466, 120},
    {392, 150},
    {330, 190},
    {0, 80}
};

static const note_t pong_wall_sfx[] = {
    {400, 45},
    {0, 20}
};

static const note_t pong_paddle_sfx[] = {
    {800, 45},
    {0, 20}
};

static const note_t snake_eat_sfx[] = {
    {740, 35},
    {988, 45},
    {0, 20}
};

static const note_t snake_die_sfx[] = {
    {220, 80},
    {180, 120},
    {0, 40}
};

static const note_t snake_move_sfx[] = {
    {330, 18},
    {0, 12}
};

static const note_t flappy_jump_sfx[] = {
    {900, 28},
    {1100, 24},
    {0, 16}
};

static const note_t flappy_point_sfx[] = {
    {784, 35},
    {1046, 55},
    {0, 20}
};

static const note_t flappy_die_sfx[] = {
    {330, 55},
    {262, 70},
    {196, 110},
    {0, 35}
};

static bool menu_active = false;
static const note_t *active_sequence = NULL;
static size_t active_length = 0;
static size_t active_index = 0;
static bool menu_sequence = false;
static absolute_time_t note_deadline;

static void audio_stop_tone(void) {
    // Para o tom atual do buzzer.
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

static void audio_start_tone(uint16_t freq) {
    if (freq == 0) {
        audio_stop_tone();
        return;
    }

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t top = (uint32_t)(125000000.0f / (AUDIO_CLOCK_DIV * (float)freq)) - 1;

    if (top > 65534u) {
        top = 65534u;
    }
    if (top < 100u) {
        top = 100u;
    }

    pwm_set_wrap(slice_num, (uint16_t)top);
    pwm_set_gpio_level(BUZZER_PIN, top / AUDIO_DUTY_DIVISOR);
}

static void audio_begin_sequence(const note_t *sequence, size_t length, bool is_menu_sequence) {
    active_sequence = sequence;
    active_length = length;
    active_index = 0;
    menu_sequence = is_menu_sequence;
    note_deadline = nil_time;
}

void audio_init(void) {
    // Configura o buzzer como saída PWM para gerar tons.
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_clkdiv(slice_num, AUDIO_CLOCK_DIV);
    pwm_set_enabled(slice_num, true);
    audio_stop_tone();
}

void audio_set_menu_active(bool active) {
    menu_active = active;
    if (!active) {
        if (menu_sequence) {
            active_sequence = NULL;
            active_length = 0;
            active_index = 0;
            menu_sequence = false;
            audio_stop_tone();
        }
        return;
    }
}

void audio_play_select(void) {
    audio_begin_sequence(select_sfx, sizeof(select_sfx) / sizeof(select_sfx[0]), false);
}

void audio_play_point(void) {
    audio_begin_sequence(point_sfx, sizeof(point_sfx) / sizeof(point_sfx[0]), false);
}

void audio_play_game_over(void) {
    audio_begin_sequence(game_over_sfx, sizeof(game_over_sfx) / sizeof(game_over_sfx[0]), false);
}

void audio_play_pong_wall(void) {
    audio_begin_sequence(pong_wall_sfx, sizeof(pong_wall_sfx) / sizeof(pong_wall_sfx[0]), false);
}

void audio_play_pong_paddle(void) {
    audio_begin_sequence(pong_paddle_sfx, sizeof(pong_paddle_sfx) / sizeof(pong_paddle_sfx[0]), false);
}

void audio_play_snake_eat(void) {
    audio_begin_sequence(snake_eat_sfx, sizeof(snake_eat_sfx) / sizeof(snake_eat_sfx[0]), false);
}

void audio_play_snake_die(void) {
    audio_begin_sequence(snake_die_sfx, sizeof(snake_die_sfx) / sizeof(snake_die_sfx[0]), false);
}

void audio_play_snake_move(void) {
    if (active_sequence == NULL || menu_sequence) {
        audio_begin_sequence(snake_move_sfx, sizeof(snake_move_sfx) / sizeof(snake_move_sfx[0]), false);
    }
}

void audio_play_flappy_jump(void) {
    audio_begin_sequence(flappy_jump_sfx, sizeof(flappy_jump_sfx) / sizeof(flappy_jump_sfx[0]), false);
}

void audio_play_flappy_point(void) {
    audio_begin_sequence(flappy_point_sfx, sizeof(flappy_point_sfx) / sizeof(flappy_point_sfx[0]), false);
}

void audio_play_flappy_die(void) {
    audio_begin_sequence(flappy_die_sfx, sizeof(flappy_die_sfx) / sizeof(flappy_die_sfx[0]), false);
}

void audio_update(void) {
    // Toca a sequência de notas no tempo correto.
    if (active_sequence == NULL) {
        return;
    }

    if (!is_nil_time(note_deadline) && absolute_time_diff_us(get_absolute_time(), note_deadline) > 0) {
        return;
    }

    if (active_index >= active_length) {
        if (menu_sequence && menu_active) {
            active_index = 0;
        } else {
            active_sequence = NULL;
            active_length = 0;
            active_index = 0;
            menu_sequence = false;
            audio_stop_tone();
            return;
        }
    }

    audio_start_tone(active_sequence[active_index].freq);
    note_deadline = delayed_by_ms(get_absolute_time(), active_sequence[active_index].duration_ms);
    active_index++;
}
