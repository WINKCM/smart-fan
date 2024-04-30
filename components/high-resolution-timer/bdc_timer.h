#ifndef _bdc_timer_h
#define _bdc_timer_h

#include "stdbool.h"

#include "esp_timer.h"

typedef struct
{
    uint64_t bdc_timer_loop_trigger_time_ms;
    esp_timer_handle_t bdc_timer_handle;
} bdc_timer_t;

bdc_timer_t *bdc_timer_create(void *bdc_timer_callback, char *bdc_timer_name, uint64_t bdc_timer_loop_trigger_time_ms, void *user_data);
void bdc_timer_start(bdc_timer_t *bdc_timer);
void bdc_timer_stop(bdc_timer_t *bdc_timer);
void bdc_timer_delete(bdc_timer_t *bdc_timer);

#endif
