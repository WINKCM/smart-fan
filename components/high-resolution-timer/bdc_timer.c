#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"

#include "bdc_timer.h"

const static char *TAG = "esp timer";

#define BDC_TIMER_INTERVAK_TIME_MS 1000

bdc_timer_t *bdc_timer_create(void *bdc_timer_callback, char *bdc_timer_name, uint64_t bdc_timer_loop_trigger_time_ms, void *user_data)
{
    if (bdc_timer_callback == NULL || bdc_timer_loop_trigger_time_ms <= 0)
        return NULL;

    bdc_timer_t *bdc_timer = malloc(sizeof(bdc_timer_t));
    bdc_timer->bdc_timer_loop_trigger_time_ms = bdc_timer_loop_trigger_time_ms;

    const esp_timer_create_args_t bdc_timer_args = {
        .callback = bdc_timer_callback,
        .arg = user_data,
        .name = bdc_timer_name};

    esp_timer_create(&bdc_timer_args, &bdc_timer->bdc_timer_handle);

    return bdc_timer;
}

void bdc_timer_start(bdc_timer_t *bdc_timer)
{
    if (bdc_timer == NULL)
        return;

    esp_timer_start_periodic(bdc_timer->bdc_timer_handle, bdc_timer->bdc_timer_loop_trigger_time_ms * 1000);
}

void bdc_timer_stop(bdc_timer_t *bdc_timer)
{
    if (bdc_timer == NULL)
        return;

    esp_timer_stop(bdc_timer->bdc_timer_handle);
}

void bdc_timer_delete(bdc_timer_t *bdc_timer)
{
    if (bdc_timer == NULL)
        return;

    esp_timer_delete(bdc_timer->bdc_timer_handle);

    bdc_timer->bdc_timer_handle = NULL;
    bdc_timer->bdc_timer_loop_trigger_time_ms = 0;

    free(bdc_timer);
    bdc_timer = NULL;
}
