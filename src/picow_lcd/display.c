#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lcd_sdk/lcd.h"
#include "lcd_sdk/icon.h"
#include "http_weather/http_weather.h"

void init_display_weather(const WeatherTimeData *data) {
    lcd_init();
    lcd_fill_screen(rgb565(255, 255, 255)); // 흰색
    
    lcd_draw_text_mixed(5, 5, data->temperature, rgb565(25, 25, 112), rgb565(255, 255, 255));
    lcd_draw_text_mixed(5, 22, data->weather, rgb565(25, 25, 112), rgb565(255, 255, 255));
    lcd_draw_text_mixed(30, 43, data->date, rgb565(25, 25, 112), rgb565(255, 255, 255));
    lcd_draw_signnum_string_scaled(2, 58, data->time, rgb565(25, 25, 112), rgb565(255, 255, 255), 3);
    
    if (strcmp(data->lcd_weather, "1") == 0 || strcmp(data->lcd_weather, "5") == 0) {
        lcd_draw_bitmap_scaled(40, 105, 50, 50, umbrella_bitmap, 1);
    }
    else if (strcmp(data->lcd_weather, "3") == 0 || strcmp(data->lcd_weather, "7") == 0) {
        lcd_draw_bitmap_scaled(40, 105, 50, 50, snow_bitmap, 1);
    }
    else if (strcmp(data->lcd_weather, "0") == 0) {
        lcd_draw_bitmap_scaled(40, 105, 50, 50, sunny_bitmap, 1);
    }
}

void display_time_update(const WeatherTimeData *data) {
    lcd_draw_signnum_string_scaled(2, 58, data->time, rgb565(25, 25, 112), rgb565(255, 255, 255), 3);
}