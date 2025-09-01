/**
 *
 * @file:       lcd_1602.c
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:      Applied source code and static functions for the LCD 1602.
 * @addtogroup @lcd_1602_driver
 *  @{
 -------------------------------------------------------------------------------------------------*/

#include "internal/lcd_1602_internal.h"

/**
 * @brief Sends a nibble (half-byte) using the i2c bus.
 * 
 * @param handle Device handle for the i2c bus
 * @param nibble half byte to be sent
 * @param rs true = char and false = command
 * 
 * @return 0 for success and else for fail.
 */
static uint8_t write_nibble(i2c_master_dev_handle_t handle, uint8_t nibble, bool rs) {
    uint8_t data = (nibble & 0xF0);

    if(rs) data |= LCD_1602_RS;

    data |= LCD_1602_BACKLIGHT;

    uint8_t sequence[2];
    sequence[0] = data | LCD_1602_ENABLE;
    sequence[1] = data & ~LCD_1602_ENABLE;
    
    return i2c_master_transmit(handle, sequence, sizeof(sequence), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief sends command with proper delays between broadcasts
 * 
 * @param handle Device handle for the i2c bus
 * @param cmd Byte for command to be send. Specified commands exists in lcd_1602_internal.h
 * 
 * @return 0 for success, else for fail.
 */
static uint8_t send_command(i2c_master_dev_handle_t handle, uint8_t cmd) {
    uint8_t err = 0;

    uint8_t high = cmd & 0xF0;
    uint8_t low = (cmd << 4) & 0xF0;

    err = write_nibble(handle, high, false);
    vTaskDelay(pdMS_TO_TICKS(2));
    err = write_nibble(handle, low, false);
    vTaskDelay(pdMS_TO_TICKS(2));

    return err;
}

/**
 * @brief sends a character with proper delays between broadcasts
 * 
 * @param handle Device handle for the i2c bus
 * @param c character to be sent
 * 
 * @return 0 for success, else for fail.
 */
 uint8_t lcd_1602_send_char(i2c_master_dev_handle_t handle, char c) {
    uint8_t err = 0;

    uint8_t high = c & 0xF0;
    uint8_t low = (c << 4) & 0xF0;

    err = write_nibble(handle, high, true);
    vTaskDelay(pdMS_TO_TICKS(2));
    err = write_nibble(handle, low, true);
    vTaskDelay(pdMS_TO_TICKS(2));

    return err;
}

/**
 * @brief Clear screen and returns DDRAM address to 0 (first row, first character)
 * 
 * @param handle Device handle for the i2c buss
 * 
 * @return 0 for success, else for fail.
 */
 uint8_t lcd_1602_clear_screen(i2c_master_dev_handle_t handle) {
    uint8_t err = send_command(handle, LCD_1602_CLEAR_SCREEN);
    vTaskDelay(pdMS_TO_TICKS(10));
    return err;
}

/**
 * @brief Sets the DDRAM address to specified position on the screen
 * 
 * @param handle Device handle for the i2c bus
 * @param x column on the device
 * @param y row on the device
 * 
 * @return 0 for success, else for fail.
 */
static uint8_t lcd_goto(i2c_master_dev_handle_t handle, uint8_t x, uint8_t y) {
    static const uint8_t row_offsets[] = { 0x00, 0x40 };
    if (y > 1) y = 1;
    uint8_t err = send_command(handle, 0x80 | (row_offsets[y] + x)); 
    
    vTaskDelay(pdMS_TO_TICKS(10));
    return err;
}

LCD_WRITE_STATUS lcd_1602_send_string(i2c_master_dev_handle_t handle, char *str) {
    lcd_1602_clear_screen(handle);

    uint8_t char_len = 0;
    uint8_t row = 0;

    while(*str != '\0') {
        if(*str == '\n' || char_len >= 16) {
            if(row < LCD_1602_MAX_ROWS) {
                lcd_goto(handle, 0,1);
                row++;
                char_len = 0;
                str++;
                continue;
            }
            else return LCD_WRITE_INTERRUPTED;
        }

        if(*str == '\0') {
            return LCD_WRITE_FINISHED;
        }

        lcd_1602_send_char(handle, *str);
        char_len++;
        str++;
    }

    return LCD_WRITE_FINISHED;
}

uint8_t lcd_1602_init(i2c_master_dev_handle_t handle) {
/*
This function sets up the standard mode of the LCD and follows
a specific start up sequence described by the manufacturer.
*/
    // Manufacturer specific order
    vTaskDelay(pdMS_TO_TICKS(15));
    write_nibble(handle, (0x03 << 4), 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    write_nibble(handle, (0x03 << 4), 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    write_nibble(handle, (0x03 << 4), 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    write_nibble(handle, (0x02 << 4), 0);
    // End of manufacturer specific order

    send_command(handle, LCD_1602_FUNCTION_SET(LCD_1602_DATA_LEN_4_BIT, LCD_1602_2_ROWS, LCD_1602_FONT_5X10));
    send_command(handle, LCD_1602_CONFIG_DISPLAY_SWITCH(LCD_1602_DISPLAY_ON, LCD_1602_CURSOR_OFF, LCD_1602_N_BLINK_DISPLAY));
    lcd_1602_clear_screen(handle);
    send_command(handle, LCD_1602_CONFIG_INPUT_SET(LCD_1602_INCREMENT_MODE, LCD_1602_CURSOR_N_MOVE));

    return 0;
}

/** @} lcd_1602_driver */