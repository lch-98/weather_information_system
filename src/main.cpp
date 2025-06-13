#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "http_weather/http_weather.h"     // 날씨 모듈 헤더
#include "picow_lcd/lcd_sdk/lcd.h"         // 디스플레이 관련 헤더
#include "picow_lcd/display.h"             // display() 함수 정의되어 있다고 가정
#include "ntp_client.h"

time_t base_time;
absolute_time_t base_ticks;

void update_time_loop(WeatherTimeData* weather) {
    int last_fetched_hour = -1;

    while (true) {
        int64_t elapsed_sec = absolute_time_diff_us(base_ticks, get_absolute_time()) / 1000000;
        time_t current_time = base_time + elapsed_sec;

        // 시간을 계산해서 weather->time 데이터를 계속 업데이트 시켜줌
        struct tm *t = localtime(&current_time);
        snprintf(weather->time, sizeof(weather->time), "%02d:%02d", t->tm_hour, t->tm_min);

        //매 3시간마다 갱신 (예: 00시, 03시, 06시 ...)
        if (t->tm_min == 0 && t->tm_hour % 3 == 0 && t->tm_hour != last_fetched_hour) {
            last_fetched_hour = t->tm_hour;

            if (fetch_weather_data(weather, &base_time, &base_ticks)) {
                init_display_weather(weather); // LCD에 다시 표시
            } else {
                printf("날씨 데이터 재갱신 실패!\n");
            }
        }

        display_time_update(weather);
        sleep_ms(1000);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(3000);

    WeatherTimeData weather;

    // 날씨 정보 가져오기
    if (!fetch_weather_data(&weather, &base_time, &base_ticks)) {
        printf("날씨 데이터를 가져오는 데 실패했습니다.\n");
        return 1;
    }
    
    // 화면 초기화 출력
    init_display_weather(&weather); // display_weather는 display.c에 존재
    // LCD 시간 갱신 루프 시작
    update_time_loop(&weather);

    return 0;
}