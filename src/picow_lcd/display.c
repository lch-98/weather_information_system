#include "pico/stdlib.h"
#include "lcd_sdk/lcd.h"
#include "http_weather/http_weather.h"

// bit map (16*16)
const uint16_t umbrella_bitmap[16 * 16] = {
    // 여기에 256개의 RGB565 색상값 넣으세요
    [0 ... 255] = 0xFFE0
};

void display_weather(const WeatherData *data) {
    lcd_init();
    lcd_fill_screen(rgb565(0, 0, 0)); // 검정

    lcd_draw_text(10, 20, "Robot Engineer!!", rgb565(255, 255, 0), rgb565(0, 0, 0));
    lcd_draw_bitmap_scaled(48, 64, 16, 16, umbrella_bitmap, 2);

    while (1);
}