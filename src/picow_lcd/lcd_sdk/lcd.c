#include "lcd.h"
#include "font5x7.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define LCD_CS   13
#define LCD_DC    8
#define LCD_RST  12
#define LCD_SCK  10
#define LCD_MOSI 11

void lcd_send_cmd(uint8_t cmd) {
    gpio_put(LCD_DC, 0);
    gpio_put(LCD_CS, 0);
    spi_write_blocking(spi1, &cmd, 1);
    gpio_put(LCD_CS, 1);
}

void lcd_send_data(uint8_t data) {
    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);
    spi_write_blocking(spi1, &data, 1);
    gpio_put(LCD_CS, 1);
}

void lcd_reset() {
    gpio_put(LCD_RST, 0);
    sleep_ms(100);
    gpio_put(LCD_RST, 1);
    sleep_ms(100);
}

void lcd_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    lcd_send_cmd(0x2A); // Column
    lcd_send_data(x0 >> 8); lcd_send_data(x0 & 0xFF);
    lcd_send_data(x1 >> 8); lcd_send_data(x1 & 0xFF);

    lcd_send_cmd(0x2B); // Row
    lcd_send_data(y0 >> 8); lcd_send_data(y0 & 0xFF);
    lcd_send_data(y1 >> 8); lcd_send_data(y1 & 0xFF);

    lcd_send_cmd(0x2C); // Write
}

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;

    lcd_set_addr_window(x, y, x, y);
    lcd_send_data(color >> 8);
    lcd_send_data(color & 0xFF);
}

void lcd_fill_screen(uint16_t color) {
    lcd_set_addr_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++) {
        lcd_send_data(color >> 8);
        lcd_send_data(color & 0xFF);
    }
}

void lcd_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg) {
    if (c < 32 || c > 126) c = '?';
    const uint8_t *bitmap = font5x7[c - 32];

    for (int col = 0; col < 5; col++) {
        uint8_t line = bitmap[col];
        for (int row = 0; row < 7; row++) {
            uint16_t color = (line & (1 << row)) ? fg : bg;
            lcd_draw_pixel(x + col, y + row, color);
        }
    }

    // 글자 간격용 1픽셀
    for (int row = 0; row < 7; row++) {
        lcd_draw_pixel(x + 5, y + row, bg);
    }
}

void lcd_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg) {
    while (*str) {
        lcd_draw_char(x, y, *str++, fg, bg);
        x += 6; // 5px + 1 spacing
    }
}

// 원래 크기대로 bitmap을 그려주는 함수
void lcd_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *bitmap) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) return;
    if (x + w > LCD_WIDTH) w = LCD_WIDTH - x;
    if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;

    lcd_set_addr_window(x, y, x + w - 1, y + h - 1);

    gpio_put(LCD_DC, 1);
    gpio_put(LCD_CS, 0);

    for (int i = 0; i < w * h; i++) {
        uint8_t data[2] = { bitmap[i] >> 8, bitmap[i] & 0xFF };
        spi_write_blocking(spi1, data, 2);
    }

    gpio_put(LCD_CS, 1);
}

// bitmap을 크게 확대 해주는 함수
void lcd_draw_bitmap_scaled(int x, int y, int w, int h, const uint16_t *data, int scale) {
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            uint16_t color = data[row * w + col];
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    lcd_draw_pixel(x + col * scale + dx, y + row * scale + dy, color);
                }
            }
        }
    }
}

uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) |
           ((g & 0xFC) << 3) |
           (b >> 3);
}

void lcd_init() {
    gpio_init(LCD_CS);  gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_init(LCD_DC);  gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_init(LCD_RST); gpio_set_dir(LCD_RST, GPIO_OUT);

    spi_init(spi1, 10 * 1000 * 1000);
    gpio_set_function(LCD_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI, GPIO_FUNC_SPI);

    lcd_reset();

    lcd_send_cmd(0x11); sleep_ms(120); // Sleep out
    lcd_send_cmd(0x36); lcd_send_data(0xC0); // MADCTL
    lcd_send_cmd(0x3A); lcd_send_data(0x05); // 16bit
    lcd_send_cmd(0x29); // Display ON
}