#include "stdio.h"
#include "stdlib.h"

#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"

#include "iot_button.h"

#include "aio_key.h"

#define AIO_KEY_MIN_LONG_PRESS_TRIGGER_TIME_MS 500
#define AIO_KEY_MAX_LONG_PRESS_TRIGGER_TIME_MS 5000
#define AIO_KEY_DEFAULT_LONG_PRESS_TRIGGER_TIME_MS 2000

#define AIO_KEY_MIN_SHORT_PRESS_TRIGGER_TIME_MS 20
#define AIO_KEY_MAX_SHORT_PRESS_TRIGGER_TIME_MS 500
#define AIO_KEY_DEFAULT_SHORT_PRESS_TRIGGER_TIME_MS 200

const static char *TAG = "aio key";

typedef struct
{
    button_handle_t button_handle;

    int key_gpio_num;
    int key_keep_gpio_num;
    uint16_t long_press_trigger_time_ms;

    bool is_power_on;

    uint16_t short_press_trigger_time_ms;
    void *short_press_callback_user_data;
    aio_key_short_press_callback_t *short_press_callback;

    bool led_control_enable;
    led_control_handle_t led_control_handle;

} aio_key_t;

static void button_long_press_start_callback(void *button_handle, void *user_data)
{
    aio_key_t *aio_key = (aio_key_t *)user_data;

    aio_key->is_power_on = !aio_key->is_power_on;
    gpio_set_level(aio_key->key_keep_gpio_num, aio_key->is_power_on);

    aio_key->is_power_on == true ? led_control_switch_led_state(aio_key->led_control_handle, LED_CONTROL_STATE_ON)
                                 : led_control_switch_led_state(aio_key->led_control_handle, LED_CONTROL_STATE_OFF);
    ESP_LOGI(TAG, "power set");
}

aio_key_handle_t aio_key_create(aio_key_config_t aio_key_config)
{
    if (aio_key_config.key_gpio_num < 0 || aio_key_config.key_keep_gpio_num < 0)
        return NULL;

    aio_key_t *aio_key = malloc(sizeof(aio_key_t));

    if (aio_key_config.led_control_enable)
    {
        aio_key->led_control_handle = led_control_create(aio_key_config.led_control_config);
        if (aio_key->led_control_handle == NULL)
        {
            ESP_LOGW(TAG, "LED control config don't set");
            // free(aio_key);
            // aio_key = NULL;
            // return NULL;
        }

        led_control_task_start(aio_key->led_control_handle);
    }

    aio_key->key_gpio_num = aio_key_config.key_gpio_num;
    aio_key->key_keep_gpio_num = aio_key_config.key_keep_gpio_num;

    gpio_set_level(aio_key->key_keep_gpio_num, 0);

    gpio_config_t io_config;
    io_config.intr_type = GPIO_INTR_DISABLE;
    io_config.pin_bit_mask = (1ULL << aio_key->key_keep_gpio_num);
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_config);

    aio_key->long_press_trigger_time_ms =                                                      //
        (aio_key_config.long_press_trigger_time_ms < AIO_KEY_MIN_LONG_PRESS_TRIGGER_TIME_MS || //
         aio_key_config.long_press_trigger_time_ms > AIO_KEY_MAX_LONG_PRESS_TRIGGER_TIME_MS)
            ? AIO_KEY_DEFAULT_LONG_PRESS_TRIGGER_TIME_MS
            : aio_key_config.long_press_trigger_time_ms;

    aio_key->short_press_trigger_time_ms =                                                       //
        (aio_key_config.short_press_trigger_time_ms < AIO_KEY_MIN_SHORT_PRESS_TRIGGER_TIME_MS || //
         aio_key_config.short_press_trigger_time_ms > AIO_KEY_MAX_SHORT_PRESS_TRIGGER_TIME_MS)
            ? AIO_KEY_DEFAULT_SHORT_PRESS_TRIGGER_TIME_MS
            : aio_key_config.short_press_trigger_time_ms;

    aio_key->is_power_on = false;

    aio_key->short_press_callback_user_data = aio_key_config.short_press_callback_user_data;
    aio_key->short_press_callback = aio_key_config.short_press_callback;

    aio_key->led_control_enable = aio_key_config.led_control_enable;

    button_config_t button_config = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = aio_key->long_press_trigger_time_ms,
        .short_press_time = aio_key->short_press_trigger_time_ms,
        .gpio_button_config = {
            .gpio_num = aio_key->key_gpio_num,
            .active_level = 0,
        },
    };

    aio_key->button_handle = iot_button_create(&button_config);
    iot_button_register_cb(aio_key->button_handle, BUTTON_LONG_PRESS_START, button_long_press_start_callback, aio_key);

    if (aio_key_config.short_press_callback_enable)
        iot_button_register_cb(aio_key->button_handle, BUTTON_PRESS_DOWN, aio_key->short_press_callback, aio_key->short_press_callback_user_data);

    return (void *)aio_key;
}

aio_key_error_t aio_key_delete(aio_key_handle_t aio_key_handle)
{
    aio_key_t *aio_key = (aio_key_t *)aio_key_handle;

    if (aio_key == NULL)
        return AIO_KEY_DELETE_ERROR;

    if (iot_button_delete(aio_key->button_handle) != ESP_OK)
        return AIO_KEY_DELETE_ERROR;
    aio_key->button_handle = NULL;

    if (led_control_task_stop(aio_key->led_control_handle) != LED_CONTROL_OK)
        return AIO_KEY_DELETE_ERROR;

    if (led_control_delete(aio_key->led_control_handle) != LED_CONTROL_OK)
        return AIO_KEY_DELETE_ERROR;
    aio_key->led_control_handle = NULL;

    aio_key->key_gpio_num = 0;
    aio_key->long_press_trigger_time_ms = 0;
    aio_key->short_press_trigger_time_ms = 0;

    aio_key->is_power_on = false;

    aio_key->short_press_callback_user_data = NULL;
    aio_key->short_press_callback = NULL;

    aio_key->led_control_enable = false;

    free(aio_key);

    return AIO_KEY_OK;
}

aio_key_error_t aio_key_switch_led_state(aio_key_handle_t aio_key_handle, aio_key_led_state_t led_state)
{
    aio_key_t *aio_key = (aio_key_t *)aio_key_handle;
    if (aio_key == NULL)
        return AIO_KEY_SWITCH_LED_STATE_ERROR;

    if (!aio_key->led_control_enable)
        return AIO_KEY_SWITCH_LED_STATE_ERROR;

    led_control_switch_led_state(aio_key->led_control_handle, led_state);

    return AIO_KEY_OK;
}

aio_key_error_t aio_key_switch_power_state(aio_key_handle_t aio_key_handle, bool power_is_on)
{
    aio_key_t *aio_key = (aio_key_t *)aio_key_handle;
    if (aio_key == NULL)
        return AIO_KEY_SWITCH_LED_STATE_ERROR;

    gpio_set_level(aio_key->key_keep_gpio_num, power_is_on);

    return AIO_KEY_OK;
}
