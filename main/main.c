#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lvgl.h"

#include "smart_fan_lvgl.h"
#include "bdc_motor.h"
#include "pcnt_driver.h"
#include "st7735.h"

const static char *TAG = "main";

void app_main(void)
{
    lv_init();
    lv_port_disp_init();

    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        lv_task_handler();
    }
}
