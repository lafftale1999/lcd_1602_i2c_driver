#ifndef LCD_1602_H_
#define LCD_1602_H_

#define DEVICE_ADDRESS  0x27
#define LCD_1602_SCREEN_CHAR_WIDTH 16
#define LCD_1602_MAX_ROWS 2

#include "internal/lcd_i2c.h"

typedef enum {
    LCD_WRITE_FINISHED,
    LCD_WRITE_NOT_FINISHED,
    LCD_WRITE_INTERRUPTED,
    LCD_TOO_LONG_STRING
} LCD_WRITE_STATUS;

LCD_WRITE_STATUS lcd_1602_send_string(i2c_master_dev_handle_t handle, char *str);
uint8_t lcd_1602_init(i2c_master_dev_handle_t handle);

#endif