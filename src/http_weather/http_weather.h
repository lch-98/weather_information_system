// http_weather 데이터를 받는 헤더
// display.h 에 데이터를 전송하는 헤더
// src/http_weather/http_weather.h
#ifndef HTTP_WEATHER_H
#define HTTP_WEATHER_H

#include <stdbool.h>

typedef struct {
    char temperature[8];  // 예: "23.5"
    char weather[16];     // 예: "맑음", "비", etc
} WeatherData;

bool fetch_weather_data(WeatherData *data);

#endif // HTTP_WEATHER_H