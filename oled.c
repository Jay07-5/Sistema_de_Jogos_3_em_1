#include "oled.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>

// oled.c é o driver do display OLED.
// Ele mantém um buffer de pixels em memória e envia para o display via I2C.

#include "hardware/gpio.h"

#define OLED_PAGES (HEIGHT / 8)
#define OLED_BUF_SIZE (WIDTH * OLED_PAGES)

static uint8_t oled_buffer[OLED_BUF_SIZE];
static uint8_t oled_address = OLED_ADDR;
static i2c_inst_t *oled_i2c = OLED_I2C;
static bool oled_ready = false;

static const uint8_t font_digits[10][5] = {
    {0x3E, 0x51, 0x49, 0x45, 0x3E},
    {0x00, 0x42, 0x7F, 0x40, 0x00},
    {0x42, 0x61, 0x51, 0x49, 0x46},
    {0x21, 0x41, 0x45, 0x4B, 0x31},
    {0x18, 0x14, 0x12, 0x7F, 0x10},
    {0x27, 0x45, 0x45, 0x45, 0x39},
    {0x3C, 0x4A, 0x49, 0x49, 0x30},
    {0x01, 0x71, 0x09, 0x05, 0x03},
    {0x36, 0x49, 0x49, 0x49, 0x36},
    {0x06, 0x49, 0x49, 0x29, 0x1E}
};

static const uint8_t font_upper[26][5] = {
    {0x7E, 0x11, 0x11, 0x11, 0x7E},
    {0x7F, 0x49, 0x49, 0x49, 0x36},
    {0x3E, 0x41, 0x41, 0x41, 0x22},
    {0x7F, 0x41, 0x41, 0x22, 0x1C},
    {0x7F, 0x49, 0x49, 0x49, 0x41},
    {0x7F, 0x09, 0x09, 0x09, 0x01},
    {0x3E, 0x41, 0x49, 0x49, 0x7A},
    {0x7F, 0x08, 0x08, 0x08, 0x7F},
    {0x00, 0x41, 0x7F, 0x41, 0x00},
    {0x20, 0x40, 0x41, 0x3F, 0x01},
    {0x7F, 0x08, 0x14, 0x22, 0x41},
    {0x7F, 0x40, 0x40, 0x40, 0x40},
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    {0x7F, 0x04, 0x08, 0x10, 0x7F},
    {0x3E, 0x41, 0x41, 0x41, 0x3E},
    {0x7F, 0x09, 0x09, 0x09, 0x06},
    {0x3E, 0x41, 0x51, 0x21, 0x5E},
    {0x7F, 0x09, 0x19, 0x29, 0x46},
    {0x46, 0x49, 0x49, 0x49, 0x31},
    {0x01, 0x01, 0x7F, 0x01, 0x01},
    {0x3F, 0x40, 0x40, 0x40, 0x3F},
    {0x1F, 0x20, 0x40, 0x20, 0x1F},
    {0x7F, 0x20, 0x18, 0x20, 0x7F},
    {0x63, 0x14, 0x08, 0x14, 0x63},
    {0x03, 0x04, 0x78, 0x04, 0x03},
    {0x61, 0x51, 0x49, 0x45, 0x43}
};

static bool oled_try_address(i2c_inst_t *i2c, uint8_t address) {
    // Tenta iniciar comunicação com o display no endereço I2C fornecido.
    uint8_t packet[2] = {0x00, 0xAE};
    int result = i2c_write_blocking(i2c, address, packet, 2, false);
    return result >= 0;
}

static bool oled_configure_bus(i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint8_t preferred_addr) {
    static const uint8_t candidates[2] = {0x3C, 0x3D};
    uint8_t ordered[2] = {preferred_addr, preferred_addr == 0x3C ? 0x3D : 0x3C};

    i2c_init(i2c, OLED_I2C_BAUDRATE);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    sleep_ms(20);

    for (size_t i = 0; i < count_of(ordered); i++) {
        uint8_t address = ordered[i];
        if (address != candidates[0] && address != candidates[1]) {
            continue;
        }

        if (oled_try_address(i2c, address)) {
            oled_i2c = i2c;
            oled_address = address;
            return true;
        }
    }

    i2c_deinit(i2c);
    return false;
}

static void oled_detect_bus_and_address(void) {
    struct oled_candidate_t {
        i2c_inst_t *i2c;
        uint sda_pin;
        uint scl_pin;
    };

    static const struct oled_candidate_t candidates[] = {
        {OLED_I2C, OLED_SDA_PIN, OLED_SCL_PIN},
        {i2c0, 0, 1},
        {i2c0, 4, 5},
        {i2c1, 2, 3},
    };

    for (size_t i = 0; i < count_of(candidates); i++) {
        if (oled_configure_bus(candidates[i].i2c, candidates[i].sda_pin, candidates[i].scl_pin, OLED_ADDR)) {
            oled_ready = true;
            return;
        }
    }

    oled_ready = false;
}

static void oled_send_command(uint8_t command) {
    uint8_t packet[2] = {0x00, command};
    if (!oled_ready) {
        return;
    }
    i2c_write_blocking(oled_i2c, oled_address, packet, 2, false);
}

static void oled_send_data(const uint8_t *data, size_t length) {
    uint8_t packet[17];
    packet[0] = 0x40;

    if (!oled_ready) {
        return;
    }

    while (length > 0) {
        size_t chunk = length > 16 ? 16 : length;
        memcpy(&packet[1], data, chunk);
        i2c_write_blocking(oled_i2c, oled_address, packet, chunk + 1, false);
        data += chunk;
        length -= chunk;
    }
}

static void oled_set_pixel(int x, int y, bool color) {
    // Define um pixel no buffer em memória. Não envia imediatamente ao display.
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
    }

    uint16_t index = (uint16_t)x + (uint16_t)(y / 8) * WIDTH;
    uint8_t mask = (uint8_t)(1u << (y % 8));

    if (color) {
        oled_buffer[index] |= mask;
    } else {
        oled_buffer[index] &= (uint8_t)~mask;
    }
}

static const uint8_t *oled_glyph_for_char(char ch) {
    static const uint8_t glyph_space[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    static const uint8_t glyph_colon[5] = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const uint8_t glyph_plus[5] = {0x08, 0x08, 0x3E, 0x08, 0x08};
    static const uint8_t glyph_dash[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const uint8_t glyph_exclamation[5] = {0x00, 0x00, 0x5F, 0x00, 0x00};

    if (ch >= '0' && ch <= '9') {
        return font_digits[ch - '0'];
    }

    if (ch >= 'A' && ch <= 'Z') {
        return font_upper[ch - 'A'];
    }

    switch (ch) {
        case ':':
            return glyph_colon;
        case '+':
            return glyph_plus;
        case '-':
            return glyph_dash;
        case '!':
            return glyph_exclamation;
        case ' ':
        default:
            return glyph_space;
    }
}

static void oled_draw_char(int x, int y, char raw_ch, bool color) {
    // Desenha um caractere usando fontes bitmap fixas.
    char ch = (char)toupper((unsigned char)raw_ch);
    const uint8_t *glyph = oled_glyph_for_char(ch);

    for (int col = 0; col < 5; col++) {
        for (int row = 0; row < 7; row++) {
            bool pixel_on = ((glyph[col] >> row) & 0x01u) != 0;
            if (pixel_on) {
                oled_set_pixel(x + col, y + row, color);
            }
        }
    }
}

static void oled_update_sh1106(void) {
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        uint8_t lower_col = (uint8_t)(OLED_COLUMN_OFFSET & 0x0F);
        uint8_t upper_col = (uint8_t)(0x10 | ((OLED_COLUMN_OFFSET >> 4) & 0x0F));

        oled_send_command((uint8_t)(0xB0 + page));
        oled_send_command(lower_col);
        oled_send_command(upper_col);
        oled_send_data(&oled_buffer[page * WIDTH], WIDTH);
    }
}

static void oled_update_ssd1306(void) {
    oled_send_command(0x20);
    oled_send_command(0x00);
    oled_send_command(0x21);
    oled_send_command(0x00);
    oled_send_command(0x7F);
    oled_send_command(0x22);
    oled_send_command(0x00);
    oled_send_command(0x07);
    oled_send_data(oled_buffer, sizeof(oled_buffer));
}

static void oled_draw_boot_pattern(void) {
    oled_clear();
    oled_rect(0, 0, WIDTH, HEIGHT, 1);
    oled_rect(2, 2, WIDTH - 4, HEIGHT - 4, 0);
    oled_rect(6, 6, WIDTH - 12, HEIGHT - 12, 1);
    oled_rect(10, 10, WIDTH - 20, HEIGHT - 20, 0);
    oled_text(10, 20, "SISTEMA DE JOGOS", 1);
    oled_text(28, 34, "PICO W", 1);
    oled_update();
    sleep_ms(700);
    oled_clear();
    oled_update();
}

void oled_init(void) {
    // Inicializa o display OLED e configura o modo de exibição.
    sleep_ms(100);
    oled_detect_bus_and_address();

    if (!oled_ready) {
        return;
    }

    oled_send_command(0xAE);
    oled_send_command(0x20);
#if OLED_DRIVER == OLED_DRIVER_SH1106
    oled_send_command(0x02);
#else
    oled_send_command(0x00);
#endif
    oled_send_command(0xB0);
    oled_send_command(0x00);
    oled_send_command(0x10);
    oled_send_command(0xA1);
    oled_send_command(0xC8);
    oled_send_command(0x40);
    oled_send_command(0xA6);
    oled_send_command(0xA8);
    oled_send_command(0x3F);
    oled_send_command(0xA4);
    oled_send_command(0xD3);
    oled_send_command(0x00);
    oled_send_command(0xD5);
    oled_send_command(0x80);
    oled_send_command(0x8D);
    oled_send_command(0x14);
    oled_send_command(0xD9);
    oled_send_command(0xF1);
    oled_send_command(0xDA);
    oled_send_command(0x12);
    oled_send_command(0x81);
    oled_send_command(0xCF);
    oled_send_command(0xDB);
    oled_send_command(0x40);
    oled_send_command(0xAF);

    oled_draw_boot_pattern();
}

bool oled_is_ready(void) {
    return oled_ready;
}

void oled_clear(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
}

void oled_update(void) {
    // Envia o buffer atual para o display físico.
    if (!oled_ready) {
        return;
    }

#if OLED_DRIVER == OLED_DRIVER_SH1106
    oled_update_sh1106();
#elif OLED_DRIVER == OLED_DRIVER_SSD1306
    oled_update_ssd1306();
#else
    oled_update_ssd1306();
    oled_update_sh1106();
#endif
}

void oled_rect(int x, int y, int w, int h, int color) {
    // Desenha um retângulo preenchido no buffer do display.
    if (w <= 0 || h <= 0) {
        return;
    }

    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            oled_set_pixel(px, py, color != 0);
        }
    }
}

void oled_text(int x, int y, const char *text, int color) {
    if (text == NULL) {
        return;
    }

    while (*text != '\0') {
        oled_draw_char(x, y, *text, color != 0);
        x += 6;
        text++;
    }
}
