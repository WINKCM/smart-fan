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
