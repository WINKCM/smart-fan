#include "stdio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "st7735.h"

const static char *TAG = "main";

void app_main(void)
{

    while (1)
    {
        ESP_LOGI(TAG, "Free heap memory: %ld bytes", esp_get_free_heap_size());
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
