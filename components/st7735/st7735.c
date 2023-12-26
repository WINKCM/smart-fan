#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"

#include "spi_master_driver.h"
#include "st7735.h"

const static char *TAG = "st7735";

#ifdef CONFIG_IDF_TARGET_ESP32
#define LCD_HOST VSPI_HOST
// #define LCD_HOST HSPI_HOST
#elif defined CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define LCD_HOST SPI2_HOST
#elif defined CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
#define LCD_HOST SPI2_HOST
#endif

#define ST7735_SCREEN_WIDTH 80
#define ST7735_SCREEN_LENGTH 160
#define ST7735_SCREEN_X_OFFSET 26
#define ST7735_SCREEN_Y_OFFSET 1

#define USE_HORIZONTAL 0

st7735_t *st7735_create()
{
    if (CONFIG_ST7735_SDA_PIN == -1 || CONFIG_ST7735_SCL_PIN == -1 || CONFIG_ST7735_CS_PIN == -1 || CONFIG_ST7735_RES_PIN == -1 || CONFIG_ST7735_DC_PIN == -1)
    {
        ESP_LOGE(TAG, "st7735 menuconfig config error");
        return NULL;
    }

    st7735_t *st7735 = malloc(sizeof(st7735_t));
    st7735->display_angle = CONFIG_ST7735_SCREEN_DISPLAY_ANGLE;

    st7735->screen_buffer = malloc(ST7735_SCREEN_WIDTH * ST7735_SCREEN_LENGTH * 2);
    memset(st7735->screen_buffer, 0, ST7735_SCREEN_WIDTH * ST7735_SCREEN_LENGTH * 2);

    st7735->spi_transaction_is_cmd = 0;
    st7735->spi_transaction_is_data = 1;
    st7735->spi_device_handle = NULL;

    ESP_LOGI(TAG, "create st7735 screen  wide %d, length %d  success ", ST7735_SCREEN_WIDTH, ST7735_SCREEN_LENGTH);

    return st7735;
}

void st7735_delete(st7735_t *st7735)
{
}

void st7735_gpio_init(int st7735_gpio)
{
    if (st7735_gpio == -1)
        return;

    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << st7735_gpio);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = true;
    gpio_config(&io_conf);
}

void st7735_hardware_restart()
{
    gpio_set_level(CONFIG_ST7735_RES_PIN, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    gpio_set_level(CONFIG_ST7735_RES_PIN, 1);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void st7735_spi_transaction_buffer_init(st7735_t *st7735)
{
    for (int i = 0; i < 6; i++)
        memset(&st7735->spi_transaction[i], 0, sizeof(spi_transaction_t));

    st7735->spi_transaction[0].length = 8;
    st7735->spi_transaction[0].tx_data[0] = 0x2a;
    st7735->spi_transaction[0].user = (void *)&st7735->spi_transaction_is_cmd;
    st7735->spi_transaction[0].flags = SPI_TRANS_USE_TXDATA;

    st7735->spi_transaction[1].length = 8 * 4;
    st7735->spi_transaction[1].tx_data[0] = 0x00;
    st7735->spi_transaction[1].tx_data[1] = ST7735_SCREEN_X_OFFSET;
    st7735->spi_transaction[1].tx_data[2] = 0x00;
    st7735->spi_transaction[1].tx_data[3] = ST7735_SCREEN_X_OFFSET + ST7735_SCREEN_WIDTH - 1;
    st7735->spi_transaction[1].user = (void *)&st7735->spi_transaction_is_data;
    st7735->spi_transaction[1].flags = SPI_TRANS_USE_TXDATA;

    st7735->spi_transaction[2].length = 8;
    st7735->spi_transaction[2].tx_data[0] = 0x2b;
    st7735->spi_transaction[2].user = (void *)&st7735->spi_transaction_is_cmd;
    st7735->spi_transaction[2].flags = SPI_TRANS_USE_TXDATA;

    st7735->spi_transaction[3].length = 8 * 4;
    st7735->spi_transaction[3].tx_data[0] = 0x00;
    st7735->spi_transaction[3].tx_data[1] = ST7735_SCREEN_Y_OFFSET;
    st7735->spi_transaction[3].tx_data[2] = 0x00;
    st7735->spi_transaction[3].tx_data[3] = ST7735_SCREEN_Y_OFFSET + ST7735_SCREEN_LENGTH - 1;
    st7735->spi_transaction[3].user = (void *)&st7735->spi_transaction_is_data;
    st7735->spi_transaction[3].flags = SPI_TRANS_USE_TXDATA;

    st7735->spi_transaction[4].length = 8;
    st7735->spi_transaction[4].tx_data[0] = 0x2c;
    st7735->spi_transaction[4].user = (void *)&st7735->spi_transaction_is_cmd;
    st7735->spi_transaction[4].flags = SPI_TRANS_USE_TXDATA;

    st7735->spi_transaction[5].length = ST7735_SCREEN_WIDTH * ST7735_SCREEN_LENGTH * 2 * 8;
    st7735->spi_transaction[5].tx_buffer = (void *)st7735->screen_buffer;
    st7735->spi_transaction[5].user = (void *)&st7735->spi_transaction_is_data;
}

st7735_error_t st7735_init(st7735_t *st7735)
{
    if (st7735 == NULL)
        return ST7735_INIT_ERROR;

    bool error = spi_master_init(&st7735->spi_device_handle, (ST7735_SCREEN_LENGTH * ST7735_SCREEN_LENGTH + 5) * 2 * 8);
    if (!error)
        return ST7735_INIT_ERROR;

    st7735_spi_transaction_buffer_init(st7735);

    st7735_gpio_init(CONFIG_ST7735_DC_PIN);
    st7735_gpio_init(CONFIG_ST7735_BLK_PIN);
    st7735_gpio_init(CONFIG_ST7735_RES_PIN);

    gpio_set_level(CONFIG_ST7735_BLK_PIN, 1);

    st7735_hardware_restart();

    spi_device_acquire_bus(st7735->spi_device_handle, portMAX_DELAY);

    spi_master_send_cmd(st7735->spi_device_handle, 0x11); // Sleep out
    vTaskDelay(120 / portTICK_PERIOD_MS);                 // Delay 120ms
    spi_master_send_cmd(st7735->spi_device_handle, 0xB1); // Normal mode
    spi_master_write_data(st7735->spi_device_handle, 0x05);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_send_cmd(st7735->spi_device_handle, 0xB2); // Idle mode
    spi_master_write_data(st7735->spi_device_handle, 0x05);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_send_cmd(st7735->spi_device_handle, 0xB3); // Partial mode
    spi_master_write_data(st7735->spi_device_handle, 0x05);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_write_data(st7735->spi_device_handle, 0x05);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_write_data(st7735->spi_device_handle, 0x3C);
    spi_master_send_cmd(st7735->spi_device_handle, 0xB4); // Dot inversion
    spi_master_write_data(st7735->spi_device_handle, 0x03);
    spi_master_send_cmd(st7735->spi_device_handle, 0xC0); // AVDD GVDD
    spi_master_write_data(st7735->spi_device_handle, 0xAB);
    spi_master_write_data(st7735->spi_device_handle, 0x0B);
    spi_master_write_data(st7735->spi_device_handle, 0x04);
    spi_master_send_cmd(st7735->spi_device_handle, 0xC1);   // VGH VGL
    spi_master_write_data(st7735->spi_device_handle, 0xC5); // C0
    spi_master_send_cmd(st7735->spi_device_handle, 0xC2);   // Normal Mode
    spi_master_write_data(st7735->spi_device_handle, 0x0D);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_send_cmd(st7735->spi_device_handle, 0xC3); // Idle
    spi_master_write_data(st7735->spi_device_handle, 0x8D);
    spi_master_write_data(st7735->spi_device_handle, 0x6A);
    spi_master_send_cmd(st7735->spi_device_handle, 0xC4); // Partial+Full
    spi_master_write_data(st7735->spi_device_handle, 0x8D);
    spi_master_write_data(st7735->spi_device_handle, 0xEE);
    spi_master_send_cmd(st7735->spi_device_handle, 0xC5); // VCOM
    spi_master_write_data(st7735->spi_device_handle, 0x0F);
    spi_master_send_cmd(st7735->spi_device_handle, 0xE0); // positive gamma
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x0E);
    spi_master_write_data(st7735->spi_device_handle, 0x08);
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x10);
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x02);
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x09);
    spi_master_write_data(st7735->spi_device_handle, 0x0F);
    spi_master_write_data(st7735->spi_device_handle, 0x25);
    spi_master_write_data(st7735->spi_device_handle, 0x36);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, 0x08);
    spi_master_write_data(st7735->spi_device_handle, 0x04);
    spi_master_write_data(st7735->spi_device_handle, 0x10);
    spi_master_send_cmd(st7735->spi_device_handle, 0xE1); // negative gamma
    spi_master_write_data(st7735->spi_device_handle, 0x0A);
    spi_master_write_data(st7735->spi_device_handle, 0x0D);
    spi_master_write_data(st7735->spi_device_handle, 0x08);
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x0F);
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x02);
    spi_master_write_data(st7735->spi_device_handle, 0x07);
    spi_master_write_data(st7735->spi_device_handle, 0x09);
    spi_master_write_data(st7735->spi_device_handle, 0x0F);
    spi_master_write_data(st7735->spi_device_handle, 0x25);
    spi_master_write_data(st7735->spi_device_handle, 0x35);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, 0x09);
    spi_master_write_data(st7735->spi_device_handle, 0x04);
    spi_master_write_data(st7735->spi_device_handle, 0x10);

    spi_master_send_cmd(st7735->spi_device_handle, 0xFC);
    spi_master_write_data(st7735->spi_device_handle, 0x80);

    spi_master_send_cmd(st7735->spi_device_handle, 0x3A);
    spi_master_write_data(st7735->spi_device_handle, 0x05);
    spi_master_send_cmd(st7735->spi_device_handle, 0x36);
    spi_master_write_data(st7735->spi_device_handle, 0x08);
    // if (USE_HORIZONTAL == 0)
    //     spi_master_write_data(st7735->spi_device_handle, 0x08);
    // else if (USE_HORIZONTAL == 1)
    //     spi_master_write_data(st7735->spi_device_handle, 0xC8);
    // else if (USE_HORIZONTAL == 2)
    //     spi_master_write_data(st7735->spi_device_handle, 0x78);
    // else if (USE_HORIZONTAL == 3)
    //     spi_master_write_data(st7735->spi_device_handle, 0xA8);
    spi_master_send_cmd(st7735->spi_device_handle, 0x21); // Display inversion
    spi_master_send_cmd(st7735->spi_device_handle, 0x29); // Display on
    spi_master_send_cmd(st7735->spi_device_handle, 0x2A); // Set Column Address
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, 0x1A); // 26
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, 0x69); // 105
    spi_master_send_cmd(st7735->spi_device_handle, 0x2B);   // Set Page Address
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, 0x01); // 1
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, 0xA0); // 160
    spi_master_send_cmd(st7735->spi_device_handle, 0x2C);

    spi_device_release_bus(st7735->spi_device_handle);
    return ST7735_OK;
}

void st7735_refresh_screen(st7735_t *st7735)
{
    spi_device_acquire_bus(st7735->spi_device_handle, portMAX_DELAY);

    spi_master_send_cmd(st7735->spi_device_handle, 0x2a);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, ST7735_SCREEN_X_OFFSET);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, ST7735_SCREEN_X_OFFSET + ST7735_SCREEN_WIDTH - 1);
    spi_master_send_cmd(st7735->spi_device_handle, 0x2b);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, ST7735_SCREEN_Y_OFFSET);
    spi_master_write_data(st7735->spi_device_handle, 0x00);
    spi_master_write_data(st7735->spi_device_handle, ST7735_SCREEN_Y_OFFSET + ST7735_SCREEN_LENGTH - 1);
    spi_master_send_cmd(st7735->spi_device_handle, 0x2c);

    esp_err_t esp_error = spi_device_polling_transmit(st7735->spi_device_handle, &st7735->spi_transaction[5]);
    assert(esp_error == ESP_OK);

    spi_device_release_bus(st7735->spi_device_handle);
}

void st7735_draw_full_screen_by_color(st7735_t *st7735, uint16_t color)
{
    uint8_t data_buffer[2];
    data_buffer[0] = color >> 8 & 0xff;
    data_buffer[1] = color & 0xff;

    for (int i = 0; i < ST7735_SCREEN_WIDTH * ST7735_SCREEN_LENGTH * 2; i += 2)
    {
        st7735->screen_buffer[i] = data_buffer[0];
        st7735->screen_buffer[i + 1] = data_buffer[1];
    }

    st7735_refresh_screen(st7735);
}

uint16_t st7735_draw_screen_by_lvgl_buffer[160 * 80];

void st7735_draw_screen_by_lvgl(st7735_t *st7735, void *color_buffer, int colcor_buffer_size)
{
    memcpy(st7735_draw_screen_by_lvgl_buffer, color_buffer, colcor_buffer_size);

    for (int x = 0; x < 80; x++)
    {
        for (int y = 0; y < 160; y++)
        {
            memcpy(st7735->screen_buffer + (y * 80 + (79 - x)) * 2, st7735_draw_screen_by_lvgl_buffer + (x * 160 + y), 2);
            // memcpy(st7735->screen_buffer + (y * 80 + x) * 2, st7735_draw_screen_by_lvgl_buffer + (x * 160 + y), 2);
        }
    }

    // memcpy(st7735->screen_buffer, st7735_draw_screen_by_lvgl_buffer, colcor_buffer_size);
    // memcpy(st7735->screen_buffer, color_buffer, colcor_buffer_size);
    st7735_refresh_screen(st7735);
}
