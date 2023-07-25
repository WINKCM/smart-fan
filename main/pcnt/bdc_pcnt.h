#ifndef _bdc_pcnt_h
#define _bdc_pcnt_h

#include "driver/pulse_cnt.h"

typedef struct
{
    pcnt_unit_handle_t pcnt_unit_a;
} bdc_pcnt_t;

bdc_pcnt_t *bdc_pcnt_create();
int bdc_pcnt_a_get_count(bdc_pcnt_t *bdc_pcnt);
void bdc_pcnt_delete(bdc_pcnt_t *bdc_pcnt);

#endif
