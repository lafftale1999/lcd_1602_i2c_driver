#include <stdio.h>
#include "lcd_1602.h"

void app_main(void)
{
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    
    i2c_open(&bus_handle, &dev_handle, DEVICE_ADDRESS);

    lcd_1602_init(dev_handle);

    while(true) {
        lcd_1602_send_string(dev_handle, "Hello world\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}