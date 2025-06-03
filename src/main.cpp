#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "http_weather/http_weather.h"     // 날씨 모듈 헤더
#include "picow_lcd/lcd_sdk/lcd.h"         // 디스플레이 관련 헤더
#include "picow_lcd/display.h"             // display() 함수 정의되어 있다고 가정

int main() {
    stdio_init_all();
    sleep_ms(3000);

    WeatherData weather;

    // 날씨 정보 가져오기
    if (!fetch_weather_data(&weather)) {
        printf("날씨 데이터를 가져오는 데 실패했습니다.\n");
        return 1;
    }

    printf(">>> 온도: %s°C\n", weather.temperature);
    printf(">>> 날씨: %s\n", weather.weather);

    // 화면 출력
    display_weather(&weather); // display_weather는 display.c에 정의되어야 함

    while (true) {
        sleep_ms(1000); // 대기 루프
    }

    return 0;
}