#include "stdio.h"
#include "stdbool.h"
#include "string.h"

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

const static char *TAG = "spi master driver";

void spi_set_dc_callback(spi_transaction_t *spi_transaction)
{
    int *st7735_dc_state = (int *)spi_transaction->user;
    gpio_set_level(CONFIG_ST7735_DC_PIN, *st7735_dc_state);
}

bool spi_master_init(spi_device_handle_t *spi_device_handle, int spi_max_send_data_lenth)
{
    spi_bus_config_t spi_bus_config = {
        .miso_io_num = -1,
        .mosi_io_num = CONFIG_ST7735_SDA_PIN,
        .sclk_io_num = CONFIG_ST7735_SCL_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = spi_max_send_data_lenth};

    spi_device_interface_config_t spi_device_interface_config = {
        .flags = SPI_DEVICE_NO_DUMMY,
        .clock_speed_hz = SPI_MASTER_FREQ_80M,
        // .clock_speed_hz = SPI_MASTER_FREQ_80M,
        .mode = 0,
        .queue_size = 7,
        .spics_io_num = CONFIG_ST7735_CS_PIN,
        .pre_cb = spi_set_dc_callback};

    esp_err_t error = spi_bus_initialize(HSPI_HOST, &spi_bus_config, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(error);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "ST7789 spi init error");
        return false;
    }

    error = spi_bus_add_device(HSPI_HOST, &spi_device_interface_config, spi_device_handle);
    ESP_ERROR_CHECK(error);
    if (error != ESP_OK)
    {
        ESP_LOGE(TAG, "ST7789 spi add device error");
        return false;
    }

    return true;
}

void spi_master_send_cmd(spi_device_handle_t spi_device_handle, uint8_t cmd)
{
    spi_transaction_t spi_transaction;
    memset(&spi_transaction, 0, sizeof(spi_transaction_t));

    spi_transaction.length = 8;
    spi_transaction.tx_buffer = &cmd;
    int cs_state = 0;
    spi_transaction.user = (void *)&cs_state;

    esp_err_t error = spi_device_polling_transmit(spi_device_handle, &spi_transaction);
    if (error != ESP_OK)
        ESP_LOGE(TAG, "spi master send cmd error");
}

void spi_master_write_data_buffer(spi_device_handle_t spi_device_handle, uint8_t *write_data_buffer, int write_data_buffer_length)
{
    if (write_data_buffer_length <= 0)
        return;

    spi_transaction_t spi_transaction;
    memset(&spi_transaction, 0, sizeof(spi_transaction_t));

    spi_transaction.length = write_data_buffer_length * 8;
    spi_transaction.tx_buffer = write_data_buffer;
    int cs_state = 1;
    spi_transaction.user = (void *)&cs_state;

    esp_err_t error = spi_device_polling_transmit(spi_device_handle, &spi_transaction);
    if (error != ESP_OK)
        ESP_LOGE(TAG, "spi master send data error");
}

void spi_master_write_data(spi_device_handle_t spi_device_handle, uint8_t write_data)
{
    spi_master_write_data_buffer(spi_device_handle, &write_data, 1);
}
