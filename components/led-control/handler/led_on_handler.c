#include "stdio.h"

#include "led_control_mode.h"

void led_control_led_on_handler(led_control_t *led_control)
{
    led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 100);
}
