#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool run_ntp_sync(void);
time_t get_ntp_time();

#ifdef __cplusplus
}
#endif

#endif // NTP_CLIENT_H