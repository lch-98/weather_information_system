#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// lcd 크기 지정
#define LCD_WIDTH  128
#define LCD_HEIGHT 160

void lcd_init();
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_fill_screen(uint16_t color);

const uint8_t* find_hangul_bitmap(uint16_t unicode);
void lcd_draw_hangul(int x, int y, uint16_t unicode, uint16_t fg, uint16_t bg);
const uint8_t* find_signnum_bitmap(uint16_t unicode);
void lcd_draw_signnum(int x, int y, uint16_t unicode, uint16_t fg, uint16_t bg);
void lcd_draw_text_mixed(int x, int y, const char *str, uint16_t fg, uint16_t bg);

void convert_signnum_bitmap_to_rgb565(const uint8_t *bitmap, uint16_t *out, int width, int height, uint16_t fg, uint16_t bg);
void lcd_draw_bitmap_scaled(int x, int y, int w, int h, const uint16_t *data, int scale);
void lcd_draw_signnum_scaled(int x, int y, uint16_t unicode, uint16_t fg, uint16_t bg, int scale);
void lcd_draw_signnum_string_scaled(int x, int y, const char *str, uint16_t fg, uint16_t bg, int scale);

// 16-bit RGB565 색상 헬퍼
uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b);

#endif