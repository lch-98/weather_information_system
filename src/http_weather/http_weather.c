#include <stdio.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"
#include "lwip/udp.h"

#include "ntp_client.h"
#include "http_weather.h"

#include "json/jsmn.h"

#define WIFI_SSID "WIFI ID"
#define WIFI_PASS "WIFI PASSWORD"

#define API_HOST "apis.data.go.kr"
#define API_PORT 80
#define API_KEY "API KEY"

char http_request[1024];
char http_response[4096];
static int response_offset = 0;
static volatile int request_done = 0; // 요청 완료 플래그
static WeatherTimeData *global_out = NULL; // 결과 저장용 전역 포인터

// jsmn key match helper
int jsmn_eq(const char *json, jsmntok_t *tok, const char *s) {
    return (tok->type == JSMN_STRING &&
            (int)strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0);
}

// 현재 날짜 문자열 생성 (YYYYMMDD)
void get_current_date(char *buf) {
    time_t now = get_ntp_time();
    struct tm *t = localtime(&now);
    strftime(buf, 9, "%Y%m%d", t);
}

// 현재 시간 문자열 생성 (HHMM)
void get_current_hour(char *buf) {
    time_t now = get_ntp_time();
    struct tm *t = localtime(&now);
    strftime(buf, 5, "%H%M", t);
}

// 날씨 코드 -> 문자열 매핑
const char* map_weather(const char *code) {
    if (strcmp(code, "0") == 0) return "맑음";
    if (strcmp(code, "1") == 0) return "비";
    if (strcmp(code, "2") == 0) return "비/눈";
    if (strcmp(code, "3") == 0) return "눈";
    if (strcmp(code, "5") == 0) return "빗방울";
    if (strcmp(code, "6") == 0) return "빗방울눈날림";
    if (strcmp(code, "7") == 0) return "눈날림";
    return "알 수 없음";
}

void parse_json_response(const char *json_start) {
    jsmn_parser parser;
    jsmntok_t tokens[256];
    jsmn_init(&parser);
    int r = jsmn_parse(&parser, json_start, strlen(json_start), tokens, 256);
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return;
    }

    char *temp = NULL;
    char *pty = NULL;

    for (int i = 0; i < r; i++) {
        if (jsmn_eq(json_start, &tokens[i], "category")) {
            int len = tokens[i+1].end - tokens[i+1].start;
            char category[8];
            snprintf(category, sizeof(category), "%.*s", len, json_start + tokens[i+1].start);

            if (strcmp(category, "T1H") == 0 || strcmp(category, "PTY") == 0) {
                for (int j = i; j < r; j++) {
                    if (jsmn_eq(json_start, &tokens[j], "obsrValue")) {
                        int val_len = tokens[j+1].end - tokens[j+1].start;
                        char *value = strndup(json_start + tokens[j+1].start, val_len);
                        if (strcmp(category, "T1H") == 0) temp = value;
                        else if (strcmp(category, "PTY") == 0) pty = value;
                        break;
                    }
                }
            }
        }
    }

    if (global_out) { // global_out에 기온값, 날씨값 추가
        if (temp) {
            snprintf(global_out->temperature, sizeof(global_out->temperature), "기온 : %s℃", temp);
        } else {
            snprintf(global_out->temperature, sizeof(global_out->temperature), "기온 : N/A");
        }
    
        if (pty) {
            const char *weather_str = map_weather(pty);
            snprintf(global_out->weather, sizeof(global_out->weather), "날씨 : %s", weather_str);
            snprintf(global_out->lcd_weather, sizeof(global_out->lcd_weather), "%s", pty); // lcd에서 날씨 값 판별해서 사용하기 위한 배열
        } else {
            snprintf(global_out->weather, sizeof(global_out->weather), "날씨 : 알 수 없음");
        }
    }

    printf(">>> 기온: %s℃\n", temp ? temp : "N/A");
    printf(">>> 날씨: %s\n", pty ? map_weather(pty) : "알 수 없음");

    if (temp) free(temp);
    if (pty) free(pty);
}

err_t on_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    if (!p) {
        printf("Connection closed by server.\n");

        char *body = strstr(http_response, "\r\n\r\n");
        if (body) {
            body += 4;
            parse_json_response(body);
        }

        request_done = 1;
        tcp_close(pcb);
        return ERR_OK;
    }

    memcpy(http_response + response_offset, p->payload, p->len);
    response_offset += p->len;
    http_response[response_offset] = '\0';
    pbuf_free(p);
    return ERR_OK;
}

err_t on_connected(void *arg, struct tcp_pcb *pcb, err_t err) {
    if (err != ERR_OK) {
        printf("Connect failed: %d\n", err);
        return err;
    }

    printf("Connected to API server, sending request...\n");
    tcp_recv(pcb, on_recv);
    tcp_write(pcb, http_request, strlen(http_request), TCP_WRITE_FLAG_COPY);
    return ERR_OK;
}

void connect_to_api(const ip_addr_t *ipaddr) {
    struct tcp_pcb *pcb = tcp_new();
    tcp_connect(pcb, ipaddr, API_PORT, on_connected);
}

void dns_cb(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (ipaddr) {
        printf("Resolved %s to %s\n", name, ipaddr_ntoa(ipaddr));
        connect_to_api(ipaddr);
    } else {
        printf("DNS lookup failed.\n");
    }
}

bool fetch_weather_data(WeatherTimeData *out, time_t *base_time, absolute_time_t *base_ticks) {
    if (!out) return false;
    global_out = out;

    response_offset = 0;
    request_done = 0;
    memset(http_response, 0, sizeof(http_response));

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return false;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Wi-Fi connection failed\n");
        return false;
    }
    printf("Wi-Fi connected.\n");

    if (!run_ntp_sync()) {
        printf("NTP time sync failed\n");
        return false;
    }

    if (base_time && base_ticks) {
        *base_time = get_ntp_time();              // 동기화된 NTP 시간 바로 저장
        *base_ticks = get_absolute_time();        // 현재 타이머 tick도 저장
    }

    char date[9], hour[5];
    get_current_date(date);  // "YYYYMMDD"
    get_current_hour(hour);  // "HHMM"
    
    // global out에 date값 추출
    snprintf(global_out->date, sizeof(global_out->date), "%c%c/%c%c/%c%c", date[2], date[3], date[4], date[5], date[6], date[7]);
    snprintf(global_out->time, sizeof(global_out->time), "%c%c : %c%c", hour[0], hour[1], hour[2], hour[3]);

    printf(">>> 기상청 API 요청에 사용된 날짜: %s\n", date);
    printf(">>> 기상청 API 요청에 사용된 시간: %s\n", hour);

    snprintf(http_request, sizeof(http_request),
             "GET /1360000/VilageFcstInfoService_2.0/getUltraSrtNcst"
             "?serviceKey=%s&pageNo=1&numOfRows=10&dataType=JSON"
             "&base_date=%s&base_time=%s&nx=55&ny=127 HTTP/1.1\r\n"
             "Host: %s\r\nConnection: close\r\n\r\n",
             API_KEY, date, hour, API_HOST);

    ip_addr_t ipaddr;
    err_t err = dns_gethostbyname(API_HOST, &ipaddr, dns_cb, NULL);
    if (err == ERR_OK) {
        connect_to_api(&ipaddr);
    }

    while (!request_done) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    return true;
}