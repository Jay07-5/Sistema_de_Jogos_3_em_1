#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "hardware/i2c.h"

#define WIDTH 128
#define HEIGHT 64

/* OLED onboard da BitDogLab / EmbarcaTech.
 * SDA -> GP14
 * SCL -> GP15
 * I2C -> i2c1
 */
#define OLED_I2C i2c1
#define OLED_I2C_BAUDRATE 100000
#define OLED_ADDR 0x3C
#define OLED_SDA_PIN 14
#define OLED_SCL_PIN 15

#define OLED_DRIVER_SSD1306 0
#define OLED_DRIVER_SH1106 1
#define OLED_DRIVER_AUTO 2

#define OLED_DRIVER OLED_DRIVER_SSD1306
#define OLED_COLUMN_OFFSET 2

void oled_init(void);
bool oled_is_ready(void);
void oled_clear(void);
void oled_update(void);
void oled_rect(int x, int y, int w, int h, int color);
void oled_text(int x, int y, const char *text, int color);
