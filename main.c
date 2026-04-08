#include <stdlib.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "audio.h"
#include "effects.h"
#include "oled.h"
#include "game.h"
#include "web.h"

// Ponto de entrada principal do sistema.
// Inicializa o hardware, prepara o contexto do jogo e entra no loop principal.
int main(void) {
    game_context_t game;

    // Inicializa todas as interfaces padrão (UART, USB CDC, etc.).
    stdio_init_all();

    // Inicializa o rádio Wi-Fi / controle da placa Pico W para poder usar o LED.
    if (cyw43_arch_init()) {
        return 1;
    }

    // Inicializa subsistemas do jogo.
    audio_init();
    effects_init();
    oled_init();

    // Verifica se o OLED respondeu corretamente; caso contrário, pisca o LED em erro.
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

    // Inicializa o ADC para leitura do joystick.
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    // Configura os botões como entradas com pull-up.
    gpio_init(JOYSTICK_BUTTON_PIN);
    gpio_set_dir(JOYSTICK_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON_PIN);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Inicializa o gerador de números aleatórios para os jogos.
    srand((unsigned int)to_us_since_boot(get_absolute_time()));

    // Inicia o contexto de jogo com valores padrão e posição inicial.
    game_init_context(&game);
    draw_menu(&game);

    // Inicializa o módulo web que conecta o Pico W ao Wi-Fi e cria a
    // interface HTTP para controle do jogo via navegador.
    web_init(&game);

    // Atualiza o menu imediatamente para mostrar o status/IP do Wi-Fi.
    draw_menu(&game);

    // Loop principal do jogo. Atualiza áudio, efeitos e a lógica do estado atual.
    while (true) {
        audio_set_menu_active(game.estado_atual == STATE_MENU);

        // Processa conexões HTTP em tempo real e atualiza o IP mostrado no OLED.
        web_poll(&game);
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
