#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lvgl.h"

#include "smart_fan_lvgl.h"
#include "aio_key.h"
#include "hardware_driver.h"

#define FAN_PWM_GPIO 26
#define BUTTON_KEY_GPIO 14

const static char *TAG = "main";

lv_ui guider_ui;

static void button_press_down_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "button_press_down_cb");
}

static void button_long_press_down_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "button_long_press_down_cb");
}

void app_main(void)
{
    button_config_t button_config = {
        .type = BUTTON_TYPE_GPIO,
        .long_press_time = 1000,
        .short_press_time = 20,
        .gpio_button_config = {
            .gpio_num = BUTTON_KEY_GPIO,
            .active_level = 0,
        },
    };

    button_handle_t button_handle = iot_button_create(&button_config);

    iot_button_register_cb(button_handle, BUTTON_LONG_PRESS_START, button_long_press_down_cb, NULL);
    iot_button_register_cb(button_handle, BUTTON_PRESS_DOWN, button_press_down_cb, NULL);

    lv_init();
    lv_port_disp_init();

    setup_ui(&guider_ui);
    lv_label_set_text(guider_ui.screen_label_1, "0");

    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        lv_task_handler();
    }
}
