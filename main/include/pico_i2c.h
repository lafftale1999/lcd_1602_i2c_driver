#ifndef _PICO_W_I2C_H
#define _PICO_W_I2C_H

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_BAUDRATE 100000

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

uint8_t pico_i2c_open();
uint8_t i2c_write_data(const uint8_t *device_address, uint8_t *data, const size_t len, bool keepMaster);
uint8_t i2c_read_data(const uint8_t *device_address, const uint8_t *d_register, uint8_t *buf, const size_t buf_size);

#endif