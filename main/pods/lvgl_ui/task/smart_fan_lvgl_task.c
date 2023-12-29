#include "stdio.h"

#include "smart_fan_lvgl_handle.h"

void smart_fan_lvgl_task_create()
{
    xTaskCreate(vTaskCode, "NAME", STACK_SIZE, &ucParameterToPass, 5, NULL);
}

void smart_fan_lvgl_task_delete()
{
}
