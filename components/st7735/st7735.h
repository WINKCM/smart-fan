#ifndef _st7735_h
#define _st7735_h

#include "stdint.h"
#include "driver/spi_master.h"

typedef enum
{
    ST7735_OK,
    ST7735_INIT_ERROR,
} st7735_error_t;

typedef enum
{
    ST7735_DISPLAY_ANGLER_0,
    ST7735_DISPLAY_ANGLER_90,
    ST7735_DISPLAY_ANGLER_180,
    ST7735_DISPLAY_ANGLER_270,
} st7735_display_angle_t;

typedef struct
{
    uint16_t display_angle;

    uint8_t *screen_buffer;

    int spi_transaction_is_cmd;
    int spi_transaction_is_data;
    spi_device_handle_t spi_device_handle;
    spi_transaction_t spi_transaction[6];
} st7735_t;

st7735_t *st7735_create();
void st7735_delete(st7735_t *st7735);

st7735_error_t st7735_init(st7735_t *st7735);

void st7735_draw_full_screen_by_color(st7735_t *st7735, uint16_t color);
void st7735_draw_pixel(st7735_t *st7735, int x, int y, uint16_t color);
void st7735_draw_screen_by_lvgl(st7735_t *st7735, void *color_buffer, int screen_width, int screen_height, st7735_display_angle_t display_angle);
void st7735_refresh_screen(st7735_t *st7735);

#endif
