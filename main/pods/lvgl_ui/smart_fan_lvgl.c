#include "stdbool.h"
#include "string.h"

#include "esp_log.h"

#include "lvgl.h"

#include "st7735.h"
#include "smart_fan_lvgl.h"

const static char *TAG = "smart fan lvgl";

#define DISPLAY_SCREEN_WIDTH 80
#define DISPLAY_SCREEN_HEIGHT 160

// #define DISPLAY_SCREEN_WIDTH 160
// #define DISPLAY_SCREEN_HEIGHT 80

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    st7735_t *st7735 = (st7735_t *)disp_drv->user_data;
    ESP_LOGI(TAG, "x : %d , y : %d", area->x2, area->y2);

    st7735_draw_screen_by_lvgl(st7735, color_p, ((area->x2 + 1) * (area->y2 + 1)) * 2);
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
                          DISPLAY_SCREEN_WIDTH * DISPLAY_SCREEN_HEIGHT);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = DISPLAY_SCREEN_WIDTH;
    disp_drv.ver_res = DISPLAY_SCREEN_HEIGHT;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &lvgl_display_draw_buffer;
    // disp_drv.rotated = 1;
    // disp_drv.rotated = LV_DISP_ROT_NONE;
    disp_drv.rotated = LV_DISP_ROT_270;
    disp_drv.full_refresh = 1;
    disp_drv.user_data = (void *)st7735;

    lv_disp_drv_register(&disp_drv);
}
