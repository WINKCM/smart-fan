#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "bdc_motor.h"
#include "pcnt_driver.h"
#include "st7735.h"

const static char *TAG = "main";

void app_main(void)
{
    // bdc_motor_t *bdc_motor = bdc_motor_create(26, 0, 27, 1);
    // bdc_motor_set_motor_speed(bdc_motor, 0);

    // bdc_pcnt_driver_t *bdc_pcnt_driver = bdc_pcnt_create(15, 13);
    // int pcnt_count = 0;
    st7735_t *st7735 = st7735_create();
    st7735_init(st7735);
    st7735_draw_full_screen_by_color(st7735, 0x0000);

    while (1)
    {
        st7735_draw_full_screen_by_color(st7735, 0x0000);

        // ESP_LOGI(TAG, "Free heap memory: %ld bytes", esp_get_free_heap_size());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        st7735_draw_full_screen_by_color(st7735, 0xaf7d);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // pcnt_count = bdc_pcnt_get_count(bdc_pcnt_driver);
        // bdc_motor_set_motor_speed(bdc_motor, (pcnt_count * 4096) / 100);
        // bdc_motor_set_motor_speed(bdc_motor, 4096);

        // ESP_LOGI(TAG, "pcnt : %d", pcnt_count);
    }
}
