#include "internal/lcd_1602_internal.h"
#include "internal/lcd_i2c.h"

static uint8_t write_nibble(i2c_master_dev_handle_t handle, uint8_t nibble, bool rs) {
    uint8_t data = (nibble & 0xF0);

    if(rs) data |= LCD_1602_RS;

    data |= LCD_1602_BACKLIGHT;

    uint8_t sequence[2];
    sequence[0] = data | LCD_1602_ENABLE;
    sequence[1] = data & ~LCD_1602_ENABLE;

    uint8_t ret = i2c_master_transmit(handle, sequence, sizeof(sequence), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    vTaskDelay(pdMS_TO_TICKS(2));
    
    return ret;
}

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

static uint8_t send_char(i2c_master_dev_handle_t handle, char c) {
    uint8_t err = 0;

    uint8_t high = c & 0xF0;
    uint8_t low = (c << 4) & 0xF0;

    err = write_nibble(handle, high, true);
    vTaskDelay(pdMS_TO_TICKS(2));
    err = write_nibble(handle, low, true);
    vTaskDelay(pdMS_TO_TICKS(2));

    return err;
}

static uint8_t clear_screen(i2c_master_dev_handle_t handle) {
    uint8_t err = send_command(handle, LCD_1602_CLEAR_SCREEN);
    vTaskDelay(pdMS_TO_TICKS(10));
    return err;
}

static uint8_t lcd_goto(i2c_master_dev_handle_t handle, uint8_t x, uint8_t y) {
    static const uint8_t row_offsets[] = { 0x00, 0x40 };
    if (y > 1) y = 1;

    return send_command(handle, 0x80 | (row_offsets[y] + x));
}

LCD_WRITE_STATUS lcd_1602_send_string(i2c_master_dev_handle_t handle, char *str) {
    clear_screen(handle);

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

        send_char(handle, *str);
        char_len++;
        str++;
    }

    return LCD_WRITE_FINISHED;
}

uint8_t lcd_1602_init(i2c_master_dev_handle_t handle) {
    vTaskDelay(pdMS_TO_TICKS(15));
    write_nibble(handle, (0x03 << 4), 0);
    vTaskDelay(pdMS_TO_TICKS(5));
    write_nibble(handle, (0x03 << 4), 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    write_nibble(handle, (0x03 << 4), 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    write_nibble(handle, (0x02 << 4), 0);

    send_command(handle, LCD_1602_FUNCTION_SET(LCD_1602_DATA_LEN_4_BIT, LCD_1602_2_ROWS, LCD_1602_FONT_5X10));
    send_command(handle, LCD_1602_CONFIG_DISPLAY_SWITCH(LCD_1602_DISPLAY_ON, LCD_1602_CURSOR_OFF, LCD_1602_N_BLINK_DISPLAY));
    clear_screen(handle);
    send_command(handle, LCD_1602_CONFIG_INPUT_SET(LCD_1602_INCREMENT_MODE, LCD_1602_CURSOR_MOVE_ON_WRITE));

    return 0;
}