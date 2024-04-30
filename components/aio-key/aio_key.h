#ifndef aio_key_h
#define aio_key_h

#include "stdint.h"
#include "stdbool.h"

#include "iot_button.h"
#include "led_control.h"

typedef void *aio_key_handle_t;

typedef void aio_key_short_press_callback_t(void *driver_handler, void *user_data);

typedef enum
{
    AIO_KEY_LED_STATE_OFF,
    AIO_KEY_LED_STATE_ON,
    AIO_KEY_LED_STATE_BLINK,
    AIO_KEY_LED_STATE_BREATHE,
} aio_key_led_state_t;

typedef enum
{
    AIO_KEY_OK,
    AIO_KEY_DELETE_ERROR,
    AIO_KEY_SWITCH_LED_STATE_ERROR,
    AIO_KEY_POWER_SWITCH_ERROR,
} aio_key_error_t;

typedef struct
{
    int key_gpio_num;
    int key_keep_gpio_num;

    uint16_t long_press_trigger_time_ms;

    bool short_press_callback_enable;
    uint16_t short_press_trigger_time_ms;
    void *short_press_callback_user_data;
    aio_key_short_press_callback_t *short_press_callback;

    bool led_control_enable;
    led_control_config_t led_control_config;
} aio_key_config_t;

aio_key_handle_t aio_key_create(aio_key_config_t aio_key_config);
aio_key_error_t aio_key_delete(aio_key_handle_t aio_key_handle);
aio_key_error_t aio_key_switch_led_state(aio_key_handle_t aio_key_handle, aio_key_led_state_t led_state);
aio_key_error_t aio_key_switch_power_state(aio_key_handle_t aio_key_handle, bool power_is_on);

#endif
