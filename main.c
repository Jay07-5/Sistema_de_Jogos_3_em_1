#include <stdlib.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "audio.h"
#include "effects.h"
#include "oled.h"
#include "game.h"

int main(void) {
    game_context_t game;

    stdio_init_all();

    if (cyw43_arch_init()) {
        return 1;
    }

    audio_init();
    effects_init();
    oled_init();

    if (!oled_is_ready()) {
        while (true) {
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            sleep_ms(150);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            sleep_ms(150);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            sleep_ms(150);
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            sleep_ms(600);
        }
    }

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    srand((unsigned int)to_us_since_boot(get_absolute_time()));
    game_init_context(&game);
    draw_menu(&game);

    while (true) {
        audio_set_menu_active(game.estado_atual == STATE_MENU);
        audio_update();
        effects_update();

        if (game.estado_atual == STATE_MENU) {
            draw_menu(&game);
        } else if (game.estado_atual == STATE_SNAKE) {
            snake_step(&game);
        } else if (game.estado_atual == STATE_PONG) {
            pong_step(&game);
        } else if (game.estado_atual == STATE_FLAPPY) {
            flappy_step(&game);
        }
    }
}
