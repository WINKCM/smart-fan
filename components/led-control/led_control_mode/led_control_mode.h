#ifndef led_control_mode_h
#define led_control_mode_h

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "bdc_timer.h"

#include "led_control.h"

typedef struct
{
    TaskHandle_t task_handle;
    SemaphoreHandle_t semaphore;

    int led_num;
    led_control_state_t current_state;
    led_control_state_t last_state;

    bdc_timer_t *bdc_timer;
    int led_brightness;
    bool led_brightness_is_rise;

    void *led_control_pwm_user_data;
    led_control_pwm_init_callback_t *led_pwm_init_callback;
    led_control_pwm_set_duty_callback_t *led_pwm_set_duty_callback;
} led_control_t;

#endif
