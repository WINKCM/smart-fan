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

    st7735->spi_device_handle = NULL;

    ESP_LOGI(TAG, "create st7735 screen length %d , st7735 wide %d success ", ST7735_SCREEN_LENGTH, ST7735_SCREEN_WIDTH);

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

    bool error = spi_master_init(&st7735->spi_device_handle, (ST7735_SCREEN_LENGTH * ST7735_SCREEN_LENGTH + 5) * 2);
    if (!error)
        return ST7735_INIT_ERROR;

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
    if (USE_HORIZONTAL == 0)
        spi_master_write_data(st7735->spi_device_handle, 0x08);
    else if (USE_HORIZONTAL == 1)
        spi_master_write_data(st7735->spi_device_handle, 0xC8);
    else if (USE_HORIZONTAL == 2)
        spi_master_write_data(st7735->spi_device_handle, 0x78);
    else
        spi_master_write_data(st7735->spi_device_handle, 0xA8);
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

void LCD_Address_Set(st7735_t *st7735, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (USE_HORIZONTAL == 0)
    {
        spi_master_send_cmd(st7735->spi_device_handle, 0x2a); // Display inversion
        spi_master_write_data(st7735->spi_device_handle, 0x00);
        spi_master_write_data(st7735->spi_device_handle, x1 + 26);
        spi_master_write_data(st7735->spi_device_handle, 0x00);
        spi_master_write_data(st7735->spi_device_handle, x2 + 26);
        spi_master_send_cmd(st7735->spi_device_handle, 0x2b); // Display inversion
        spi_master_write_data(st7735->spi_device_handle, 0x00);
        spi_master_write_data(st7735->spi_device_handle, y1 + 1);
        spi_master_write_data(st7735->spi_device_handle, 0x00);
        spi_master_write_data(st7735->spi_device_handle, y2 + 1);
        spi_master_send_cmd(st7735->spi_device_handle, 0x2c); // Display inversion
    }
    // else if (USE_HORIZONTAL == 1)
    // {
    //     LCD_WR_REG(0x2a); // 列地址设置
    //     LCD_WR_DATA(x1 + 26);
    //     LCD_WR_DATA(x2 + 26);
    //     LCD_WR_REG(0x2b); // 行地址设置
    //     LCD_WR_DATA(y1 + 1);
    //     LCD_WR_DATA(y2 + 1);
    //     LCD_WR_REG(0x2c); // 储存器写
    // }
    // else if (USE_HORIZONTAL == 2)
    // {
    //     LCD_WR_REG(0x2a); // 列地址设置
    //     LCD_WR_DATA(x1 + 1);
    //     LCD_WR_DATA(x2 + 1);
    //     LCD_WR_REG(0x2b); // 行地址设置
    //     LCD_WR_DATA(y1 + 26);
    //     LCD_WR_DATA(y2 + 26);
    //     LCD_WR_REG(0x2c); // 储存器写
    // }
    // else
    // {
    //     LCD_WR_REG(0x2a); // 列地址设置
    //     LCD_WR_DATA(x1 + 1);
    //     LCD_WR_DATA(x2 + 1);
    //     LCD_WR_REG(0x2b); // 行地址设置
    //     LCD_WR_DATA(y1 + 26);
    //     LCD_WR_DATA(y2 + 26);
    //     LCD_WR_REG(0x2c); // 储存器写
    // }
}

void LCD_Fill(st7735_t *st7735, uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    LCD_Address_Set(st7735, xsta, ysta, xend - 1, yend - 1); // 设置显示范围
    uint8_t data_buffer[2];
    data_buffer[0] = color >> 8 & 0xff;
    data_buffer[1] = color & 0xff;

    for (int i = 0; i < ST7735_SCREEN_WIDTH * ST7735_SCREEN_LENGTH * 2; i += 2)
    {
        st7735->screen_buffer[i] = data_buffer[0];
        st7735->screen_buffer[i + 1] = data_buffer[1];
    }

    spi_master_write_data_buffer(st7735->spi_device_handle, st7735->screen_buffer, ST7735_SCREEN_WIDTH * ST7735_SCREEN_LENGTH * 2);
}

void st7735_draw_full(st7735_t *st7735, uint16_t color)
{
}
