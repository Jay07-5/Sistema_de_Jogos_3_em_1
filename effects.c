#include "effects.h"

#include "pico/stdlib.h"

#define LED_R_PIN 13
#define LED_G_PIN 12
#define LED_B_PIN 11

static bool effect_active = false;
static int effect_step = 0;
static absolute_time_t effect_deadline;

static void leds_all_off(void) {
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
}

void effects_init(void) {
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);
    leds_all_off();
}

void effects_trigger_game_over(void) {
    effect_active = true;
    effect_step = 0;
    effect_deadline = nil_time;
}

void effects_update(void) {
    if (!effect_active) {
        return;
    }

    if (!is_nil_time(effect_deadline) && absolute_time_diff_us(get_absolute_time(), effect_deadline) > 0) {
        return;
    }

    switch (effect_step) {
        case 0:
            gpio_put(LED_R_PIN, 1);
            gpio_put(LED_G_PIN, 0);
            gpio_put(LED_B_PIN, 0);
            effect_deadline = delayed_by_ms(get_absolute_time(), 70);
            break;
        case 1:
            gpio_put(LED_R_PIN, 1);
            gpio_put(LED_G_PIN, 1);
            gpio_put(LED_B_PIN, 0);
            effect_deadline = delayed_by_ms(get_absolute_time(), 70);
            break;
        case 2:
            gpio_put(LED_R_PIN, 1);
            gpio_put(LED_G_PIN, 1);
            gpio_put(LED_B_PIN, 1);
            effect_deadline = delayed_by_ms(get_absolute_time(), 90);
            break;
        case 3:
            gpio_put(LED_R_PIN, 0);
            gpio_put(LED_G_PIN, 1);
            gpio_put(LED_B_PIN, 1);
            effect_deadline = delayed_by_ms(get_absolute_time(), 70);
            break;
        case 4:
            gpio_put(LED_R_PIN, 0);
            gpio_put(LED_G_PIN, 0);
            gpio_put(LED_B_PIN, 1);
            effect_deadline = delayed_by_ms(get_absolute_time(), 70);
            break;
        default:
            leds_all_off();
            effect_active = false;
            effect_step = 0;
            effect_deadline = nil_time;
            return;
    }

    effect_step++;
}
