#include "stdio.h"

#include "esp_log.h"

#include "led_control_handler.h"
#include "led_control_mode.h"

const static char *TAG = "led control task";

#define LED_CONTROL_TASK_STACK_DEPTH 1024 * 4
#define LED_CONTROL_TASK_PRIORTY 2

void led_control_task_handler(void *arguments)
{
    led_control_t *led_control = (led_control_t *)arguments;

    while (1)
    {
        vTaskDelay(50 / portTICK_PERIOD_MS);

        if (xSemaphoreTake(led_control->semaphore, 50 / portTICK_PERIOD_MS) == pdTRUE)
        {
            if (led_control->last_state == led_control->current_state)
            {
                xSemaphoreGive(led_control->semaphore);
                continue;
            }

            switch (led_control->current_state)
            {
            case LED_CONTROL_STATE_OFF:
                led_control_led_off_handler(led_control);
                break;
            case LED_CONTROL_STATE_ON:
                led_control_led_on_handler(led_control);
                break;
            case LED_CONTROL_STATE_BLINK:
                led_control_led_blink_handler(led_control);
                break;
            case LED_CONTROL_STATE_BREATHE:
                led_control_led_breath_handler(led_control);
                break;
            }
            led_control->last_state = led_control->current_state;

            xSemaphoreGive(led_control->semaphore);
        }

        xSemaphoreGive(led_control->semaphore);
    }
}

led_control_error_t led_control_task_start(led_control_handle_t led_control_handle)
{
    led_control_t *led_control = (led_control_t *)led_control_handle;
    if (led_control == NULL)
        return LED_CONTROL_TASK_START_ERROR;

    xTaskCreate(led_control_task_handler, "led_control_task_handler", LED_CONTROL_TASK_STACK_DEPTH, led_control, LED_CONTROL_TASK_PRIORTY, &led_control->task_handle);

    return LED_CONTROL_OK;
}

led_control_error_t led_control_task_stop(led_control_handle_t led_control_handle)
{
    led_control_t *led_control = (led_control_t *)led_control_handle;
    if (led_control == NULL)
        return LED_CONTROL_TASK_STOP_ERROR;

    if (led_control->bdc_timer != NULL)
    {
        bdc_timer_stop(led_control->bdc_timer);
        bdc_timer_delete(led_control->bdc_timer);
    }
    led_control->bdc_timer = NULL;
    led_control->led_brightness = 0;

    if (xSemaphoreTake(led_control->semaphore, portMAX_DELAY) == pdTRUE)
    {
        if (led_control->task_handle != NULL)
        {
            vTaskDelete(led_control->task_handle);
            led_control->task_handle = NULL;
        }

        led_control->led_pwm_set_duty_callback(led_control->led_control_pwm_user_data, 0);
        ESP_LOGI(TAG, "led control task delete");
    }

    xSemaphoreGive(led_control->semaphore);

    return LED_CONTROL_OK;
}
