#include "stdio.h"
#include "stdbool.h"

#include "led_control_mode.h"

void led_control_led_blink_handler_callback(void *arguments)
{
    led_control_t *led_control = (led_control_t *)arguments;

    led_control->led_brightness_is_rise ?                                                 //
        led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 0) //
                                        : led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 90);

    led_control->led_brightness_is_rise = !led_control->led_brightness_is_rise;
}

void led_control_led_blink_handler(led_control_t *led_control)
{
    if (led_control->bdc_timer != NULL)
        return;

    led_control->bdc_timer = bdc_timer_create(led_control_led_blink_handler_callback, "led blink", 300, led_control);
    bdc_timer_start(led_control->bdc_timer);
}

void led_control_led_blink_close_handler(led_control_t *led_control)
{
    bdc_timer_stop(led_control->bdc_timer);
    bdc_timer_delete(led_control->bdc_timer);
    led_control->bdc_timer = NULL;

    led_control->led_brightness_is_rise = false;
    led_control->led_brightness = 0;

    led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 0);
}
