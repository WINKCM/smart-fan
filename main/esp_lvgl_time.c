#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "time.h"
#include "sys/time.h"

uint32_t custom_tick_get()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
