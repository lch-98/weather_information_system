#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd_sdk/lcd.h"
#include "lcd_sdk/icon.h"
#include "http_weather/http_weather.h"

void display_weather(const WeatherData *data) {
    lcd_init();
    lcd_fill_screen(rgb565(255, 255, 255)); // 흰색
    
    // 함수가 잘 실행되는지 디버깅
    //printf("기온: %s°C\n", data->temperature);
    //printf("날씨: %s\n", data->weather);
    lcd_draw_text_mixed(5, 10, "기온 : N/A℃ ", rgb565(25, 25, 112), rgb565(255, 255, 255));
    lcd_draw_text_mixed(5, 30, "날씨 : N/A", rgb565(25, 25, 112), rgb565(255, 255, 255));
    lcd_draw_signnum_string_scaled(4, 53, "23:55", rgb565(25, 25, 112), rgb565(255, 255, 255), 3);
    lcd_draw_bitmap_scaled(40, 105, 50, 50, umbrella_bitmap, 1);
}