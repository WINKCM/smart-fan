#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "st7735.h"

const static char *TAG = "main";

void app_main(void)
{

    st7735_t *st7735 = st7735_create();
    if (st7735 == NULL)
    {
        ESP_LOGE(TAG, "st7735 create error");
    }

    st7735_init(st7735);

    while (1)
    {

        st7735_draw_full_screen_by_color(st7735, 0xffff);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        st7735_draw_full_screen_by_color(st7735, 0x001f);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        st7735_draw_full_screen_by_color(st7735, 0x7e0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        st7735_draw_full_screen_by_color(st7735, 0xf800);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
