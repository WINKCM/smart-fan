#ifndef _spi_master_driver_h
#define _spi_master_driver_h

#include "driver/spi_master.h"

bool spi_master_init(spi_device_handle_t *spi_device_handle);
void spi_master_send_cmd(spi_device_handle_t spi_device_handle, uint8_t cmd, bool keep_cs_active);
void spi_master_write_data(spi_device_handle_t spi_device_handle, uint8_t *write_data_buffer, int write_data_buffer_length, bool keep_cs_active);

#endif
