#include "lcd_1602_internal.h"
#include "pico_i2c.h"
#include "utilities.h"

/**
 * @brief Sends a nibble (half-byte) using the i2c bus.
 * 
 * @param nibble half byte to be sent
 * @param rs true = char and false = command
 * 
 * @return 0 for success and else for fail.
 */

static uint8_t write_nibble(uint8_t nibble, bool rs) {
    uint8_t data = (nibble & 0xF0);

    if(rs) data |= LCD_1602_RS;

    data |= LCD_1602_BACKLIGHT;
    data &= ~LCD_1602_RW;

    uint8_t sequence[2];
    sequence[0] = data | LCD_1602_ENABLE;
    sequence[1] = data & ~LCD_1602_ENABLE;
    
    uint8_t device_address = DEVICE_ADDRESS;

    return i2c_write_data(&device_address, sequence, 2, false);
}

/**
 * @brief sends command with proper delays between broadcasts
 * 
 * @param cmd Byte for command to be send. Specified commands exists in lcd_1602_internal.h
 * 
 * @return 0 for success, else for fail.
 */
static uint8_t send_command(uint8_t cmd) {
    uint8_t high = cmd & 0xF0;
    uint8_t low = (cmd << 4) & 0xF0;

    uint8_t err = write_nibble(high, false);
    if(err) return err;
    non_blocking_delay_ms(2);

    err = write_nibble(low, false);
    non_blocking_delay_ms(2);

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
static uint8_t send_char(char c) {
    uint8_t high = c & 0xF0;
    uint8_t low = (c << 4) & 0xF0;

    uint8_t err = write_nibble(high, true);
    non_blocking_delay_ms(2);
    if(err) return err;

    err = write_nibble(low, true);
    non_blocking_delay_ms(2);

    return err;
}

/**
 * @brief Clear screen and returns DDRAM address to 0 (first row, first character)
 * 
 * @param handle Device handle for the i2c buss
 * 
 * @return 0 for success, else for fail.
 */
static uint8_t clear_screen() {
    uint8_t err = send_command(LCD_1602_CLEAR_SCREEN);
    non_blocking_delay_ms(10);
    return err;
}

/**
 * @brief Sets the DDRAM address to specified position on the screen
 * 
 * @param x column on the device
 * @param y row on the device
 * 
 * @return 0 for success, else for fail.
 */
static uint8_t lcd_goto(uint8_t x, uint8_t y) {
    static const uint8_t row_offsets[] = { 0x00, 0x40 };
    if (y > 1) y = 1;
    uint8_t err = send_command(0x80 | (row_offsets[y] + x)); 
    
    non_blocking_delay_ms(10);
    return err;
}

LCD_WRITE_STATUS lcd_1602_send_string(char *str) {
    clear_screen();

    uint8_t char_len = 0;
    uint8_t row = 0;

    while(*str != '\0') {
        if(*str == '\n' || char_len >= 16) {
            if(row < LCD_1602_MAX_ROWS) {
                lcd_goto(0,1);
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

        if(send_char(*str)) return LCD_WRITE_FAILED_SEND;
        char_len++;
        str++;
    }

    return LCD_WRITE_FINISHED;
}

uint8_t lcd_1602_init() {
/*
This function sets up the standard mode of the LCD and follows
a specific start up sequence described by the manufacturer.
*/
    // Manufacturer specific order
    non_blocking_delay_ms(50);
    write_nibble(0x30, false); non_blocking_delay_ms(5);
    write_nibble(0x30, false); non_blocking_delay_ms(5);
    write_nibble(0x30, false); non_blocking_delay_ms(5);
    write_nibble(0x20, false); non_blocking_delay_ms(5);
    // End of manufacturer specific order

    send_command(LCD_1602_FUNCTION_SET(LCD_1602_DATA_LEN_4_BIT, LCD_1602_2_ROWS, LCD_1602_FONT_5X8));
    send_command(LCD_1602_CONFIG_DISPLAY_SWITCH(LCD_1602_DISPLAY_ON, LCD_1602_CURSOR_OFF, LCD_1602_N_BLINK_DISPLAY));
    clear_screen();
    send_command(LCD_1602_CONFIG_INPUT_SET(LCD_1602_INCREMENT_MODE, LCD_1602_CURSOR_N_MOVE));

    return 0;
}