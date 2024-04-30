#include "stdio.h"

#include "led_control_mode.h"

void led_control_led_breath_handler_callback(void *arguments)
{
    led_control_t *led_control = (led_control_t *)arguments;

    if (led_control->led_brightness_is_rise)
        if ((led_control->led_brightness += 5) > 100)
        {
            led_control->led_brightness = 100;
            led_control->led_brightness_is_rise = !led_control->led_brightness_is_rise;
        }

    if (!led_control->led_brightness_is_rise)
        if ((led_control->led_brightness -= 5) < 0)
        {
            led_control->led_brightness = 0;
            led_control->led_brightness_is_rise = !led_control->led_brightness_is_rise;
        }

    led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, led_control->led_brightness);
}

void led_control_led_breath_handler(led_control_t *led_control)
{
    if (led_control->bdc_timer != NULL)
        return;

    led_control->bdc_timer = bdc_timer_create(led_control_led_breath_handler_callback, "led breath", 40, led_control);
    bdc_timer_start(led_control->bdc_timer);
}

void led_control_led_breath_close_handler(led_control_t *led_control)
{
    bdc_timer_stop(led_control->bdc_timer);
    bdc_timer_delete(led_control->bdc_timer);
    led_control->bdc_timer = NULL;

    led_control->led_brightness_is_rise = false;
    led_control->led_brightness = 0;
}
