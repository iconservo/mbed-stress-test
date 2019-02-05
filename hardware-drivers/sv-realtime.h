/*
 * sv-realtime.h
 */

#ifndef SV_REALTIME_H_
#define SV_REALTIME_H_

#define RTC2_RTC_XOR_VALUE 0xAAAAAAAA

/* Functions prototypes */
void time_keep_init(void);
void time_keep_refresh(void);
int rtc_get_corrupted(void);

#endif /* SV_REALTIME_H_ */
