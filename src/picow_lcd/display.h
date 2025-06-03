// http_weather.h 데이터를 받는 헤더
// display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include "http_weather/http_weather.h"

#ifdef __cplusplus
extern "C" {
#endif

void display_weather(const WeatherData *data);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H