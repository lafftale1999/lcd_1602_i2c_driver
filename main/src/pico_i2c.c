#include "pico_i2c.h"
#include <stdio.h>
#include "pico/stdlib.h"

uint8_t pico_i2c_open() {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, I2C_BAUDRATE);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    return 0;
}

uint8_t i2c_write_data(const uint8_t *device_address, uint8_t *data, const size_t len, bool keepMaster) {
    int err = i2c_write_blocking(I2C_PORT, *device_address, data, len, keepMaster);

    return (err == (int)len) ? 0 : 1;
}

uint8_t i2c_read_data(const uint8_t *device_address, const uint8_t *d_register, uint8_t *buf, const size_t buf_size) {
    // Start with a pointer to where we should start reading.
    int err = i2c_write_blocking(I2C_PORT, *device_address, d_register, 1, true);
    if (err == PICO_ERROR_GENERIC) {
        return 1;
    }

    // The BME280s data registers are autoincrementing - so start reading from a certain address ex 0xF7 (pressure MSB) -> 0xFE (humidity LSB).
    err = i2c_read_blocking(I2C_PORT, *device_address, buf, buf_size, false);
    if (err == PICO_ERROR_GENERIC) {
        return 1;
    }

    return 0;
}