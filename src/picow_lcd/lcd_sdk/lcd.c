#include "lcd.h"
#include "font.h"
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

const uint8_t* find_hangul_bitmap(uint16_t unicode) {
    for (int i = 0; i < get_hangul_font_count(); i++) {
        if (hangul_fonts[i].unicode == unicode) {
            return hangul_fonts[i].bitmap;
        }
    }
    return NULL;
}

const uint8_t* find_signnum_bitmap(uint16_t unicode) {
    for (int i = 0; i < get_signnum_font_count(); i++) {
        if (signnum_fonts[i].unicode == unicode) {
            return signnum_fonts[i].bitmap;
        }
    }
    return NULL;
}

void lcd_draw_signnum(int x, int y, uint16_t unicode, uint16_t fg, uint16_t bg) {
    const uint8_t* bitmap = find_signnum_bitmap(unicode);
    if (!bitmap) return;

    for (int row = 0; row < 16; row++) {
        uint8_t byte = bitmap[row];
        for (int col = 0; col < 8; col++) {
            uint16_t color = (byte & (0x80 >> col)) ? fg : bg;
            lcd_draw_pixel(x + col, y + row, color);
        }
    }
}

void lcd_draw_hangul(int x, int y, uint16_t unicode, uint16_t fg, uint16_t bg) {
    const uint8_t* bitmap = find_hangul_bitmap(unicode);
    if (!bitmap) return;

    for (int row = 0; row < 16; row++) {
        uint8_t high = bitmap[row * 2];     // 왼쪽 8비트
        uint8_t low  = bitmap[row * 2 + 1]; // 오른쪽 8비트

        for (int col = 0; col < 8; col++) {
            uint16_t color = (high & (0x80 >> col)) ? fg : bg;
            lcd_draw_pixel(x + col, y + row, color);
        }
        for (int col = 0; col < 8; col++) {
            uint16_t color = (low & (0x80 >> col)) ? fg : bg;
            lcd_draw_pixel(x + 8 + col, y + row, color);
        }
    }
}

void lcd_draw_text_mixed(int x, int y, const char *str, uint16_t fg, uint16_t bg) {
    while (*str) {
        uint8_t c = *str;

        // UTF-8: 한글은 3바이트로 시작이 0xE1 ~ 0xEC
        if ((c & 0xF0) == 0xE0 && str[1] && str[2]) {
            uint16_t unicode = ((str[0] & 0x0F) << 12) |
                               ((str[1] & 0x3F) << 6) |
                               (str[2] & 0x3F);
            lcd_draw_hangul(x, y, unicode, fg, bg);
            str += 3;
            x += 16;
        }
        // ASCII 문자 (1바이트)
        else {
            lcd_draw_signnum(x, y, c, fg, bg);
            str++;
            x += 8;
        }
    }
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

void convert_signnum_bitmap_to_rgb565(const uint8_t *bitmap, uint16_t *out, int width, int height, uint16_t fg, uint16_t bg) {
    for (int row = 0; row < height; row++) {
        uint8_t byte = bitmap[row];
        for (int col = 0; col < width; col++) {
            uint16_t color = (byte & (0x80 >> col)) ? fg : bg;
            out[row * width + col] = color;
        }
    }
}

void lcd_draw_signnum_scaled(int x, int y, uint16_t unicode, uint16_t fg, uint16_t bg, int scale) {
    const uint8_t *bitmap = find_signnum_bitmap(unicode);
    if (!bitmap) return;

    uint16_t buffer[8 * 16]; // 8x16 픽셀용 버퍼
    convert_signnum_bitmap_to_rgb565(bitmap, buffer, 8, 16, fg, bg);
    lcd_draw_bitmap_scaled(x, y, 8, 16, buffer, scale);
}

void lcd_draw_signnum_string_scaled(int x, int y, const char *str, uint16_t fg, uint16_t bg, int scale) {
    while (*str) {
        lcd_draw_signnum_scaled(x, y, *str, fg, bg, scale);
        x += 8 * scale;  // 문자 폭 8 * 배율만큼 이동
        str++;
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