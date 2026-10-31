/**
 *
 * @file:       lcd_1602.c
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:      Applied source code and static functions for the LCD 1602.
 * @addtogroup lcd_1602_driver
 *  @{
 -------------------------------------------------------------------------------------------------*/

#include "internal/lcd_1602_internal.h"

/** DDRAM start address of each row on a HD44780 compatible 2-row display */
static const uint8_t row_offsets[LCD_1602_MAX_ROWS] = { 0x00, 0x40 };

/**
 * @brief Blocks for at least the given number of milliseconds.
 *
 * pdMS_TO_TICKS() rounds down, so with the default 100 Hz tick a request for
 * 2 ms becomes 0 ticks and vTaskDelay() returns immediately. Adding one tick
 * guarantees that the full delay has passed, which the LCD timing requires.
 *
 * @param ms minimum time to wait in milliseconds
 */
static void lcd_delay_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms) + 1);
}

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

    /* i2c_master_transmit() expects the timeout in milliseconds, not ticks */
    esp_err_t ret = i2c_master_transmit(handle, sequence, sizeof(sequence), I2C_MASTER_TIMEOUT_MS);

    return (ret == ESP_OK) ? 0 : 1;
}

/**
 * @brief Sends a full byte as two nibbles, high nibble first.
 *
 * @param handle Device handle for the i2c bus
 * @param byte the byte to send
 * @param rs true = char and false = command
 *
 * @return 0 for success, else for fail.
 */
static uint8_t write_byte(i2c_master_dev_handle_t handle, uint8_t byte, bool rs) {
    uint8_t err = 0;

    uint8_t high = byte & 0xF0;
    uint8_t low = (byte << 4) & 0xF0;

    err |= write_nibble(handle, high, rs);
    err |= write_nibble(handle, low, rs);
    lcd_delay_ms(1);

    return err;
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
    return write_byte(handle, cmd, false);
}

uint8_t lcd_1602_send_char(i2c_master_dev_handle_t handle, char c) {
    return write_byte(handle, (uint8_t)c, true);
}

uint8_t lcd_1602_clear_screen(i2c_master_dev_handle_t handle) {
    uint8_t err = send_command(handle, LCD_1602_CLEAR_SCREEN);
    lcd_delay_ms(2);    // Clear display needs 1.52 ms to execute
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
    if (y >= LCD_1602_MAX_ROWS) y = LCD_1602_MAX_ROWS - 1;
    if (x >= LCD_1602_SCREEN_CHAR_WIDTH) x = LCD_1602_SCREEN_CHAR_WIDTH - 1;

    return send_command(handle, LCD_1602_SET_DDRAM_ADDR | (row_offsets[y] + x));
}

/**
 * @brief Writes a string to a single row without touching the other rows.
 *
 * The rest of the row is padded with spaces so that any previous content
 * on that row is erased. Writing stops at '\0', '\n' or the end of the row.
 *
 * @param handle Device handle for the i2c bus
 * @param row the row to write to (0 = top)
 * @param str the string to write
 *
 * @return LCD_WRITE_FINISHED if the whole string fit on the row,
 * LCD_TOO_LONG_STRING if it was truncated, LCD_WRITE_INTERRUPTED on a bus error.
 */
static LCD_WRITE_STATUS write_row(i2c_master_dev_handle_t handle, uint8_t row, const char *str) {
    if(str == NULL) str = "";

    uint8_t err = lcd_goto(handle, 0, row);

    uint8_t col = 0;
    while(col < LCD_1602_SCREEN_CHAR_WIDTH && *str != '\0' && *str != '\n') {
        err |= lcd_1602_send_char(handle, *str);
        str++;
        col++;
    }

    while(col < LCD_1602_SCREEN_CHAR_WIDTH) {
        err |= lcd_1602_send_char(handle, ' ');
        col++;
    }

    if(err) return LCD_WRITE_INTERRUPTED;

    /* Anything left except a single trailing newline did not fit */
    if(*str == '\n') str++;
    return (*str == '\0') ? LCD_WRITE_FINISHED : LCD_TOO_LONG_STRING;
}

LCD_WRITE_STATUS lcd_1602_write_top(i2c_master_dev_handle_t handle, const char *str) {
    return write_row(handle, 0, str);
}

LCD_WRITE_STATUS lcd_1602_write_bottom(i2c_master_dev_handle_t handle, const char *str) {
    return write_row(handle, 1, str);
}

LCD_WRITE_STATUS lcd_1602_send_string(i2c_master_dev_handle_t handle, const char *str) {
    if(str == NULL) return LCD_WRITE_FINISHED;

    uint8_t err = lcd_1602_clear_screen(handle);

    uint8_t char_len = 0;
    uint8_t row = 0;

    while(*str != '\0') {
        if(*str == '\n' || char_len >= LCD_1602_SCREEN_CHAR_WIDTH) {
            /* A newline that ends the string needs no new row */
            if(*str == '\n' && *(str + 1) == '\0') {
                break;
            }

            if(row + 1 < LCD_1602_MAX_ROWS) {
                row++;
                err |= lcd_goto(handle, 0, row);
                char_len = 0;

                if(*str == '\n') {
                    str++;
                    continue;
                }
            }
            else return LCD_WRITE_INTERRUPTED;
        }

        err |= lcd_1602_send_char(handle, *str);
        char_len++;
        str++;
    }

    return err ? LCD_WRITE_INTERRUPTED : LCD_WRITE_FINISHED;
}

uint8_t lcd_1602_init(i2c_master_dev_handle_t handle) {
/*
This function sets up the standard mode of the LCD and follows
a specific start up sequence described by the manufacturer.
*/
    uint8_t err = 0;

    // Manufacturer specific order
    lcd_delay_ms(50);   // Wait >40 ms after power on
    err |= write_nibble(handle, (0x03 << 4), false);
    lcd_delay_ms(5);    // Wait >4.1 ms
    err |= write_nibble(handle, (0x03 << 4), false);
    lcd_delay_ms(1);    // Wait >100 us
    err |= write_nibble(handle, (0x03 << 4), false);
    lcd_delay_ms(1);
    err |= write_nibble(handle, (0x02 << 4), false);
    lcd_delay_ms(1);
    // End of manufacturer specific order

    // 5x10 font is only supported in single-row mode, so 2 rows must use 5x8
    err |= send_command(handle, LCD_1602_FUNCTION_SET(LCD_1602_DATA_LEN_4_BIT, LCD_1602_2_ROWS, LCD_1602_FONT_5X7));
    err |= send_command(handle, LCD_1602_CONFIG_DISPLAY_SWITCH(LCD_1602_DISPLAY_ON, LCD_1602_CURSOR_OFF, LCD_1602_N_BLINK_DISPLAY));
    err |= lcd_1602_clear_screen(handle);
    err |= send_command(handle, LCD_1602_CONFIG_INPUT_SET(LCD_1602_INCREMENT_MODE, LCD_1602_CURSOR_N_MOVE));

    return err ? 1 : 0;
}

/** @} lcd_1602_driver */
