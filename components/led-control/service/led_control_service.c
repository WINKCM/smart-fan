#include "stdio.h"

#include "led_control_mode.h"
#include "led_control_handler.h"

led_control_error_t led_control_switch_led_state(led_control_handle_t led_control_handle, led_control_state_t led_state)
{
    led_control_t *led_control = (led_control_t *)led_control_handle;
    if (led_control == NULL)
        return LED_CONTROL_LED_SWITCH_ERROR;

    if (xSemaphoreTake(led_control->semaphore, 100 / portTICK_PERIOD_MS) == pdTRUE)
    {
        led_control->last_state = led_control->current_state;
        if (led_control->last_state == led_state)
        {
            xSemaphoreGive(led_control->semaphore);
            return LED_CONTROL_OK;
        }

        if (led_control->last_state == LED_CONTROL_STATE_BLINK)
            led_control_led_blink_close_handler(led_control);
        else if (led_control->last_state == LED_CONTROL_STATE_BREATHE)
            led_control_led_breath_close_handler(led_control);

        led_control->current_state = led_state;
        xSemaphoreGive(led_control->semaphore);
    }

    return LED_CONTROL_OK;
}
