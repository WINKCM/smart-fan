#include "stdio.h"
#include "stdbool.h"
#include "stdlib.h"

#include "led_control_mode.h"

led_control_handle_t led_control_create(led_control_config_t led_control_config)
{
    led_control_t *led_control = malloc(sizeof(led_control_t));
    if (led_control_config.led_num < 0 || led_control_config.led_pwm_init_callback == NULL || led_control_config.led_control_pwm_set_duty_callback == NULL)
        return NULL;

    led_control->task_handle = NULL;
    vSemaphoreCreateBinary(led_control->semaphore);
    led_control->led_num = led_control_config.led_num;
    led_control->current_state = LED_CONTROL_STATE_OFF;
    led_control->last_state = led_control->current_state;

    led_control->bdc_timer = NULL;
    led_control->led_brightness = 0;
    led_control->led_brightness_is_rise = false;

    led_control->led_control_pwm_user_data = led_control_config.led_control_pwm_user_data;
    led_control->led_pwm_init_callback = led_control_config.led_pwm_init_callback;
    led_control->led_pwm_set_duty_callback = led_control_config.led_control_pwm_set_duty_callback;

    led_control->led_pwm_init_callback(led_control->led_control_pwm_user_data, led_control->led_num);
    led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 0);

    return (led_control_handle_t)led_control;
}

led_control_error_t led_control_delete(led_control_handle_t led_control_handle)
{
    led_control_t *led_control = (led_control_t *)led_control_handle;

    if (led_control == NULL)
        return LED_CONTROL_DELETE_ERROR;

    if (led_control->task_handle != NULL)
        if (xSemaphoreTake(led_control->semaphore, portMAX_DELAY) == pdTRUE)
        {
            vTaskDelete(led_control->task_handle);
            led_control->task_handle = NULL;
        }

    xSemaphoreGive(led_control->semaphore);

    vSemaphoreDelete(led_control->semaphore);
    led_control->led_num = 0;
    led_control->current_state = LED_CONTROL_STATE_OFF;
    led_control->last_state = LED_CONTROL_STATE_OFF;

    if (led_control->bdc_timer != NULL)
    {
        bdc_timer_stop(led_control->bdc_timer);
        bdc_timer_delete(led_control->bdc_timer);
    }
    led_control->bdc_timer = NULL;
    led_control->led_brightness = 0;
    led_control->led_brightness_is_rise = false;
    led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 0);

    led_control->led_control_pwm_user_data = NULL;
    led_control->led_pwm_init_callback = NULL;
    led_control->led_pwm_set_duty_callback = NULL;

    free(led_control);
    led_control = NULL;

    return LED_CONTROL_OK;
}
