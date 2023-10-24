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

st7735_t *st7735_create()
{
    if (CONFIG_ST7735_SCREEN_LENGHT == -1 || CONFIG_ST7735_SCREEN_WIDE == -1 || CONFIG_ST7735_SDA_PIN == -1 || CONFIG_ST7735_SCL_PIN == -1 || CONFIG_ST7735_CS_PIN == -1 || CONFIG_ST7735_RES_PIN == -1 || CONFIG_ST7735_DC_PIN == -1)
    {
        ESP_LOGE(TAG, "st7735 menuconfig config error");
        return NULL;
    }

    st7735_t *st7735 = malloc(sizeof(st7735_t));
    st7735->length = CONFIG_ST7735_SCREEN_LENGHT;
    st7735->wide = CONFIG_ST7735_SCREEN_WIDE;
    st7735->display_angle = CONFIG_ST7735_SCREEN_DISPLAY_ANGLE;

    st7735->screen_buffer = malloc(st7735->length * st7735->wide * 2);
    memset(st7735->screen_buffer, 0, st7735->length * st7735->wide * 2);

    st7735->spi_device_handle = NULL;

    ESP_LOGI(TAG, "create st7735 screen length %d , st7735 wide %d ", st7735->length, st7735->wide);

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

st7735_error_t st7735_init(st7735_t *st7735)
{
    if (st7735 == NULL)
        return ST7735_INIT_ERROR;

    bool error = spi_master_init(&st7735->spi_device_handle);
    if (!error)
        return ST7735_INIT_ERROR;

    st7735_gpio_init(CONFIG_ST7735_DC_PIN);
    st7735_gpio_init(CONFIG_ST7735_BLK_PIN);
    st7735_gpio_init(CONFIG_ST7735_RES_PIN);

    gpio_set_level(CONFIG_ST7735_BLK_PIN, 1);

    st7735_hardware_restart();

    uint8_t st7735_init_data[16];
    memset(st7735_init_data, 0, 16);
    // int st7735_dc_state = 0;
    spi_device_acquire_bus(st7735->spi_device_handle, portMAX_DELAY);
    spi_master_send_cmd(st7735->spi_device_handle, 0x01, false);
    vTaskDelay(150 / portTICK_PERIOD_MS);
    spi_master_send_cmd(st7735->spi_device_handle, 0x11, false);
    vTaskDelay(150 / portTICK_PERIOD_MS);

    spi_master_send_cmd(st7735->spi_device_handle, 0xB1, false);
    st7735_init_data[0] = 0x01;
    st7735_init_data[1] = 0x2C;
    st7735_init_data[2] = 0x2D;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 3, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xB2, false);
    st7735_init_data[0] = 0x01;
    st7735_init_data[1] = 0x2C;
    st7735_init_data[2] = 0x2D;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 3, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xB3, false);
    st7735_init_data[0] = 0x01;
    st7735_init_data[1] = 0x2C;
    st7735_init_data[2] = 0x2D;
    st7735_init_data[3] = 0x01;
    st7735_init_data[4] = 0x2C;
    st7735_init_data[5] = 0x2D;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 6, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xB4, false);
    st7735_init_data[0] = 0X07;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 1, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xC0, false);
    st7735_init_data[0] = 0xA2;
    st7735_init_data[1] = 0x02;
    st7735_init_data[2] = 0x84;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 3, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xC1, false);
    st7735_init_data[0] = 0xC5;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 1, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xC2, false);
    st7735_init_data[0] = 0x0A;
    st7735_init_data[1] = 0x00;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 2, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xC3, false);
    st7735_init_data[0] = 0x8A;
    st7735_init_data[1] = 0x2A;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 2, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xC4, false);
    st7735_init_data[0] = 0x8A;
    st7735_init_data[1] = 0xEE;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 2, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xC5, false);
    st7735_init_data[0] = 0x0E;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 1, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0x20, false);
    st7735_init_data[0] = 0x00;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 1, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0x36, false);
    st7735_init_data[0] = 0x20;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 1, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0x3A, false);
    st7735_init_data[0] = 0x05;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 1, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0x2A, false);
    st7735_init_data[0] = 0x00;
    st7735_init_data[1] = 0x02;
    st7735_init_data[2] = 0x00;
    st7735_init_data[3] = st7735->wide + 0x02;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 4, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0x2B, false);
    st7735_init_data[0] = 0x00;
    st7735_init_data[1] = 0x01;
    st7735_init_data[2] = 0x00;
    st7735_init_data[3] = st7735->length + 0x01;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 4, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xE0, false);
    st7735_init_data[0] = 0x02;
    st7735_init_data[1] = 0x1c;
    st7735_init_data[2] = 0x07;
    st7735_init_data[3] = 0x12;
    st7735_init_data[4] = 0x37;
    st7735_init_data[5] = 0x32;
    st7735_init_data[6] = 0x29;
    st7735_init_data[7] = 0x2d;
    st7735_init_data[8] = 0x29;
    st7735_init_data[9] = 0x25;
    st7735_init_data[10] = 0x2B;
    st7735_init_data[11] = 0x39;
    st7735_init_data[12] = 0x00;
    st7735_init_data[13] = 0x01;
    st7735_init_data[14] = 0x03;
    st7735_init_data[15] = 0x10;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 16, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0xE1, false);
    st7735_init_data[0] = 0x03;
    st7735_init_data[1] = 0x1d;
    st7735_init_data[2] = 0x07;
    st7735_init_data[3] = 0x06;
    st7735_init_data[4] = 0x2E;
    st7735_init_data[5] = 0x2C;
    st7735_init_data[6] = 0x29;
    st7735_init_data[7] = 0x2d;
    st7735_init_data[8] = 0x2E;
    st7735_init_data[9] = 0x2E;
    st7735_init_data[10] = 0x37;
    st7735_init_data[11] = 0x3F;
    st7735_init_data[12] = 0x00;
    st7735_init_data[13] = 0x00;
    st7735_init_data[14] = 0x02;
    st7735_init_data[15] = 0x10;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 16, false);
    memset(st7735_init_data, 0, 16);

    spi_master_send_cmd(st7735->spi_device_handle, 0x13, false);
    spi_master_send_cmd(st7735->spi_device_handle, 0x21, false);
    spi_master_send_cmd(st7735->spi_device_handle, 0x29, false);

    spi_device_release_bus(st7735->spi_device_handle);
    return ST7735_OK;
}

void st7735_draw_full(st7735_t *st7735, uint16_t color)
{
    ESP_LOGI(TAG, "st7735_draw_full");

    uint8_t st7735_init_data[4];
    memset(st7735_init_data, 0, 4);

    memset(st7735->screen_buffer, 0, st7735->length * st7735->wide * 2);
    for (int i = 0; i < (st7735->length * st7735->wide * 2); i += 2)
    {
        st7735->screen_buffer[i] = color & 0xFF;
        st7735->screen_buffer[i + 1] = color >> 8;
    }

    spi_device_acquire_bus(st7735->spi_device_handle, portMAX_DELAY);

    spi_master_send_cmd(st7735->spi_device_handle, 0x2A, false);
    st7735_init_data[0] = 0x00;
    st7735_init_data[1] = 0x02;
    st7735_init_data[2] = 0xff;
    st7735_init_data[3] = 0xff - 0x02;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 4, false);
    memset(st7735_init_data, 0, 4);

    spi_master_send_cmd(st7735->spi_device_handle, 0x2B, false);
    st7735_init_data[0] = 0x00;
    st7735_init_data[1] = 0x01;
    st7735_init_data[2] = 0xff;
    st7735_init_data[3] = 0xff - 0x01;
    spi_master_write_data(st7735->spi_device_handle, st7735_init_data, 4, false);
    memset(st7735_init_data, 0, 4);

    spi_master_send_cmd(st7735->spi_device_handle, 0x2C, false);
    spi_master_write_data(st7735->spi_device_handle, st7735->screen_buffer, st7735->length * st7735->wide * 2, false);

    spi_device_release_bus(st7735->spi_device_handle);
}
