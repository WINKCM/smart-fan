#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lvgl.h"

#include "smart_fan_lvgl.h"
#include "pcnt_driver.h"
#include "bdc_motor.h"

const static char *TAG = "main";

lv_ui guider_ui;

void app_main(void)
{
    bdc_motor_t *bdc_motor = bdc_motor_create(26, 0, 27, 1);
    int motor_speed = 0;
    bdc_motor_set_motor_speed(bdc_motor, motor_speed);

    bdc_pcnt_driver_t *bdc_pcnt_driver = bdc_pcnt_create(15, 13);

    lv_init();
    lv_port_disp_init();

    setup_ui(&guider_ui);
    lv_label_set_text(guider_ui.screen_label_1, "0");

    int pcnt_count = 0;
    char motor_speed_string[20];
    memset(motor_speed_string, 0, 20);

    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        lv_task_handler();

        pcnt_count = bdc_pcnt_get_count(bdc_pcnt_driver);
        motor_speed = 4096 * pcnt_count / 100;
        bdc_motor_set_motor_speed(bdc_motor, motor_speed);
        sprintf(motor_speed_string, "%d", pcnt_count);
        lv_label_set_text(guider_ui.screen_label_1, motor_speed_string);
        ESP_LOGI(TAG, "pcnt count : %d", pcnt_count);
    }
}
