// http_weather 데이터를 받는 헤더
// display.h 에 데이터를 전송하는 헤더
// src/http_weather/http_weather.h
#ifndef HTTP_WEATHER_H
#define HTTP_WEATHER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { //parse_json_response, fetch_weather_data 함수에서 각각 값을 반환해줌
    char temperature[32];  // 예: "기온: 23.5°C"
    char weather[32];      // 예: "날씨: 맑음"
    char lcd_weather[2];  // lcd에서 weather값을 판별해서 사진을 찾기 위한 배열
    char date[11];         // "25/06/13"
    char time[6];          // "17:43"
} WeatherTimeData;

bool fetch_weather_data(WeatherTimeData *out, time_t *base_time, absolute_time_t *base_ticks);

#ifdef __cplusplus
}
#endif

#endif // HTTP_WEATHER_H