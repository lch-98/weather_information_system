#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/dns.h"

#include "ntp_client.h"

#define NTP_SERVER "pool.ntp.org"
#define NTP_PORT 123
#define NTP_MSG_LEN 48
#define NTP_DELTA 2208988800UL

typedef struct {
    struct udp_pcb *pcb;
    ip_addr_t server_ip;
    bool completed;
    time_t synced_time;
} ntp_state_t;

static ntp_state_t ntp_state = {0};

time_t get_ntp_time() {
    return ntp_state.synced_time;
}

static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                     const ip_addr_t *addr, u16_t port) {
    if (p->tot_len == NTP_MSG_LEN) {
        uint8_t buffer[4];
        pbuf_copy_partial(p, buffer, 4, 40);  // timestamp seconds @ offset 40
        uint32_t secs_since_1900 = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
        time_t epoch = secs_since_1900 - NTP_DELTA;

        ntp_state.synced_time = epoch;
        ntp_state.completed = true;

        struct tm *t = localtime(&epoch);
        printf("[NTP] 동기화 시간: %04d-%02d-%02d %02d:%02d:%02d\n",
               t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
               t->tm_hour, t->tm_min, t->tm_sec);
    }

    pbuf_free(p);
}

static void ntp_send_request() {
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    if (!p) return;

    uint8_t *req = (uint8_t *)p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1B; // LI=0, Version=3, Mode=3 (client)

    udp_sendto(ntp_state.pcb, p, &ntp_state.server_ip, NTP_PORT);
    pbuf_free(p);
}

static void dns_callback(const char *name, const ip_addr_t *ipaddr, void *arg) {
    if (ipaddr) {
        ntp_state.server_ip = *ipaddr;
        ntp_send_request();
    } else {
        printf("[NTP] DNS 실패: %s\n", name);
    }
}

bool run_ntp_sync(void) {
    ntp_state.completed = false;
    ntp_state.pcb = udp_new_ip_type(IPADDR_TYPE_V4);
    if (!ntp_state.pcb) {
        printf("[NTP] PCB 생성 실패\n");
        return false;
    }

    udp_recv(ntp_state.pcb, ntp_recv, NULL);

    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(NTP_SERVER, &ntp_state.server_ip, dns_callback, NULL);
    cyw43_arch_lwip_end();

    if (err == ERR_OK) {
        ntp_send_request(); // DNS 캐시 존재
    } else if (err != ERR_INPROGRESS) {
        printf("[NTP] DNS 요청 실패\n");
        return false;
    }

    // 최대 5초 동안 대기
    absolute_time_t deadline = make_timeout_time_ms(5000);
    while (!ntp_state.completed && absolute_time_diff_us(get_absolute_time(), deadline) > 0) {
        cyw43_arch_poll();
        sleep_ms(10);
    }

    udp_remove(ntp_state.pcb);

    return ntp_state.completed;
}