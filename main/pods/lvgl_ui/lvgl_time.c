#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "time.h"
#include "sys/time.h"
// #include "esp_timer.h"

// #define CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR 1
// #define LV_TICK_CUSTOM_SYS_TIME_EXPR ((esp_timer_get_time() / 1000LL))

// #define CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR
// #define LV_TICK_CUSTOM_SYS_TIME_EXPR CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR

uint32_t custom_tick_get()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
