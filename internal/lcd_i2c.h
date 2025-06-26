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

#define I2C_MASTER_SCL_IO           GPIO_NUM_2                  /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           GPIO_NUM_3                  /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              I2C_NUM_0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          100000                      /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_DEVICE_ADDRESS_LEN      I2C_ADDR_BIT_LEN_7

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