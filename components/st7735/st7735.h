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

    spi_device_handle_t spi_device_handle;
} st7735_t;

st7735_t *st7735_create();
void st7735_delete(st7735_t *st7735);

st7735_error_t st7735_init(st7735_t *st7735);
void LCD_Fill(st7735_t *st7735, uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);

void st7735_draw_pixel(st7735_t *st7735, int x, int y, uint16_t color);

void st7735_draw_full(st7735_t *st7735, uint16_t color);

#endif
