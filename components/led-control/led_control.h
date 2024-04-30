#ifndef led_control_h
#define led_control_h

#include "stdint.h"

typedef void *led_control_handle_t;

typedef void led_control_pwm_init_callback_t(void *user_data, int led_gpio_num);
typedef void led_control_pwm_set_duty_callback_t(void *user_data, int duty);

typedef enum
{
    LED_CONTROL_OK,
    LED_CONTROL_TASK_START_ERROR,
    LED_CONTROL_TASK_STOP_ERROR,
    LED_CONTROL_LED_SWITCH_ERROR,
    LED_CONTROL_DELETE_ERROR
} led_control_error_t;

typedef enum
{
    LED_CONTROL_STATE_OFF,
    LED_CONTROL_STATE_ON,
    LED_CONTROL_STATE_BLINK,
    LED_CONTROL_STATE_BREATHE,
} led_control_state_t;

typedef struct
{
    int led_num;

    void *led_control_pwm_user_data;
    led_control_pwm_init_callback_t *led_pwm_init_callback;
    led_control_pwm_set_duty_callback_t *led_control_pwm_set_duty_callback;
} led_control_config_t;

led_control_handle_t led_control_create(led_control_config_t led_control_config);
led_control_error_t led_control_task_start(led_control_handle_t led_control_handle);
led_control_error_t led_control_task_stop(led_control_handle_t led_control_handle);
led_control_error_t led_control_switch_led_state(led_control_handle_t led_control_handle, led_control_state_t led_state);
led_control_error_t led_control_delete(led_control_handle_t led_control_handle);

#endif
