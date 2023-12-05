#ifndef _pcnt_driver_h
#define _pcnt_driver_h

#include "driver/pulse_cnt.h"

typedef struct
{
    pcnt_unit_handle_t pcnt_unit;
} bdc_pcnt_driver_t;

bdc_pcnt_driver_t *bdc_pcnt_create(int edge_gpio_num, int level_gpio_num);
int bdc_pcnt_get_count(bdc_pcnt_driver_t *bdc_pcnt_driver);
void bdc_pcnt_delete(bdc_pcnt_driver_t *bdc_pcnt_driver);

#endif
