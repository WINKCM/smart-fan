#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gpio.h"

#include "lvgl.h"
#include "iot_button.h"

#include "lvgl_dispaly_register.h"
#include "hardware_driver.h"

const static char *TAG = "main";

#define FAN_PWM_GPIO 26
#define BUTTON_KEY_GPIO 14
#define POWER_KEEP_GPIO 12

lv_ui guider_ui;

static void button_press_down_cb(void *arg, void *data)
{
    int *fan_speed = (int *)data;
    *fan_speed += 10;
    if (*fan_speed > 100)
        *fan_speed = 0;

    pwm_set_duty(NULL, *fan_speed);

    char fan_speed_string[4];
    memset(fan_speed_string, 0, 4);
    sprintf(fan_speed_string, "%d", *fan_speed);
    lv_label_set_text(guider_ui.screen_label_1, fan_speed_string);
    ESP_LOGI(TAG, "%s", fan_speed_string);
    lv_bar_set_value(guider_ui.screen_bar_1, *fan_speed, LV_ANIM_OFF);

    ESP_LOGI(TAG, "fan speed %d", *fan_speed);
}

static void button_long_press_down_cb(void *arg, void *data)
{
    bool *power_is_on = (bool *)data;
    *power_is_on = !*power_is_on;
    gpio_set_level(POWER_KEEP_GPIO, *power_is_on);

    ESP_LOGI(TAG, "power is on %d", *power_is_on);
}

void app_main(void)
{
    bool power_is_on = false;
    int fan_speed = 0;

    pwm_init(NULL, FAN_PWM_GPIO);
    pwm_set_duty(NULL, fan_speed);

    gpio_config_t io_config;
    io_config.intr_type = GPIO_INTR_DISABLE;
    io_config.pin_bit_mask = (1ULL << POWER_KEEP_GPIO);
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_config.pull_up_en = GPIO_PULLUP_DISABLE;
    // io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_config);

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

    iot_button_register_cb(button_handle, BUTTON_LONG_PRESS_START, button_long_press_down_cb, &power_is_on);

    while (!power_is_on)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "power state %d", power_is_on);
    }

    iot_button_register_cb(button_handle, BUTTON_PRESS_DOWN, button_press_down_cb, &fan_speed);

    lv_init();
    lv_port_disp_init();
    setup_ui(&guider_ui);
    lv_label_set_text(guider_ui.screen_label_1, "0");
    lv_label_set_text(guider_ui.screen_label_2, "24:00");
    lv_bar_set_value(guider_ui.screen_bar_1, 0, LV_ANIM_OFF);

    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        lv_task_handler();
        lv_tick_inc(10);
        // ESP_LOGI(TAG, "is active");
    }
}
