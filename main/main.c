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
        // ESP_LOGI(TAG, "Free heap memory: %ld bytes", esp_get_free_heap_size());
        LCD_Fill(st7735, 0, 0, 80, 160, 0xffff);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        LCD_Fill(st7735, 0, 0, 80, 160, 0x0000);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
