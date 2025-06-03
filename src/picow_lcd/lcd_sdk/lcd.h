#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// lcd 크기 지정
#define LCD_WIDTH  128
#define LCD_HEIGHT 160

void lcd_init();
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void lcd_fill_screen(uint16_t color);
void lcd_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg);
void lcd_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg);
void lcd_draw_bitmap(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *bitmap);
void lcd_draw_bitmap_scaled(int x, int y, int w, int h, const uint16_t *data, int scale);

// 16-bit RGB565 색상 헬퍼
uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b);

#endif
