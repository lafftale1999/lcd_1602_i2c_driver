#ifndef LCD_I2C_H_
#define LCD_I2C_H_

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_config.h"

/**
 * @brief initializes the i2c communications and
 * points the handles to correctly initialized variables
 * on the heap.
 * 
 * @param[out] bus_handle handle for initializing the bus
 * @param[out] dev_handle handle for initializing the device on bus
 * @param[in] address device address on bus to communicate with
 */
uint8_t i2c_open(i2c_master_bus_handle_t *bus_handle, i2c_master_dev_handle_t *dev_handle, const uint8_t address);

#ifdef __cplusplus
extern "C" }
#endif

#endif