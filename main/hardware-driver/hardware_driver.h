#ifndef _hardware_init_h
#define _hardware_init_h

#include "stdint.h"

void pwm_init(void *user_data, int pwm_gpio);
void pwm_set_duty(void *user_data, int pwm_duty);

#endif
