#include "stdbool.h"
#include "string.h"

#include "esp_log.h"

#include "lvgl.h"

#include "st7735.h"
#include "smart_fan_lvgl.h"

const static char *TAG = "smart fan lvgl";

#define DISPLAY_SCREEN_WIDTH 80
#define DISPLAY_SCREEN_HEIGHT 160

// uint16_t color_p_buffer[DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT + 1];

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    // ESP_LOGI(TAG, "x : %d , y : %d", area->x2, area->y2);

    st7735_t *st7735 = (st7735_t *)disp_drv->user_data;
    // memset(color_p_buffer, 0, DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT * 2);
    // memcpy(color_p_buffer, color_p, DISPLAY_SCREEN_HEIGHT * DISPLAY_SCREEN_WIDTH * 2);

    // ESP_LOG_BUFFER_HEX(TAG, color_p_buffer, (DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT + 1) * 2);
    // int32_t x, y = 0;
    // for (y = area->y1; y <= area->y2; y++)
    // {
    //     for (x = area->x1; x <= area->x2; x++)
    //     {
    //         color_p++;
    //         // uint16_t color = lv_color_to16(*color_p);
    //         // lv_color_t color = *color_p;
    //         // esp_log_write(ESP_LOG_INFO, TAG, "%.4x ", color);
    //     }
    // }

    st7735_draw_screen_by_lvgl(st7735, color_p, area->x2 * area->y2 * 2);

    // esp_log_write(ESP_LOG_INFO, TAG, "\n");

    // ESP_LOGI(TAG, "display stop");
    lv_disp_flush_ready(disp_drv);
}

void lv_port_disp_init(void)
{
    st7735_t *st7735 = st7735_create();
    st7735_init(st7735);
    st7735_draw_full_screen_by_color(st7735, 0x0000);

    static lv_disp_draw_buf_t lvgl_display_draw_buffer;
    static lv_color_t lvgl_display_draw_buffer_1[DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT];
    static lv_color_t lvgl_display_draw_buffer_2[DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT];
    lv_disp_draw_buf_init(&lvgl_display_draw_buffer, lvgl_display_draw_buffer_1, lvgl_display_draw_buffer_2,
                          DISPLAY_SCREEN_HEIGHT * DISPLAY_SCREEN_HEIGHT);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = DISPLAY_SCREEN_WIDTH;
    disp_drv.ver_res = DISPLAY_SCREEN_HEIGHT;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &lvgl_display_draw_buffer;
    disp_drv.full_refresh = 1;
    disp_drv.user_data = (void *)st7735;

    lv_disp_drv_register(&disp_drv);
}
