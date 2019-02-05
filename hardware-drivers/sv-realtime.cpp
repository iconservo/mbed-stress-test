/*
 * sv-realtime.cpp
 */
#include "sv-realtime.h"
#include "mbed.h"
#include "mbed_critical.h"

int rtc_was_corrupted = 0;

__attribute__((section(".noinit"))) uint32_t volatile guard_var;
__attribute__((section(".noinit"))) uint32_t volatile common_time_keep;
__attribute__((section(".noinit"))) uint32_t volatile time_keep_crc;

/*
 * Init function for RTC functionality
 */
void time_keep_init(void) {
    /* Check RTC correct time */
    if (common_time_keep != (time_keep_crc ^ RTC2_RTC_XOR_VALUE)) {
        common_time_keep = 0;
        time_keep_crc = common_time_keep ^ RTC2_RTC_XOR_VALUE;
        rtc_was_corrupted = 1;
    } else {
        /* If time is correct - setup system time */
        set_time(common_time_keep);
    }
}

/*
 * Get info, if RTC2 time was corrupted after reset
 * @return - true if time was corrupted, false overwise
 */
int rtc_get_corrupted(void) {
    return rtc_was_corrupted;
}

/*
 * Sets seconds number to RTC2 and system timer
 * @param - seconds to set
 */
void time_keep_refresh(void) {
    /*
     * Interrupts  should be disabled when we write counter
     */
    core_util_critical_section_enter();
    common_time_keep = time(NULL);
    time_keep_crc = common_time_keep ^ RTC2_RTC_XOR_VALUE;
    core_util_critical_section_exit();
}
