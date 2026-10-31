/**
 *
 * @file:       lcd_1602.h
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:  External API:s to use for the LCD 1602 driver.
 * @defgroup lcd_1602_api LCD1602 External API:s
 * @ingroup lcd_1602_driver
 * @{
 * @brief All of the external API:s used the for LCD 1602 driver.
 * @details
 * Example use of this driver
 * @code
 * #include "lcd_1602.h"
 *
 * void app_main(void)
 * {
 *    i2c_master_bus_handle_t bus_handle;
 *    i2c_master_dev_handle_t dev_handle;
 *   
 *    i2c_open(&bus_handle, &dev_handle, DEVICE_ADDRESS);
 *
 *    lcd_1602_init(dev_handle);
 *
 *    while(true) {
 *        lcd_1602_send_string(dev_handle, "Hello world\n");
 *
 *        vTaskDelay(pdMS_TO_TICKS(1000));
 *    }
 * }
 * @endcode
 -------------------------------------------------------------------------------------------------*/

#ifndef LCD_1602_H_
#define LCD_1602_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/
#include "../internal/lcd_i2c.h"

/**
 * @brief configurable defines for setting up the LCD1602
 * @defgroup config_defines Configurable Defines
 * @ingroup lcd_1602_api
 * @{
 */
#define DEVICE_ADDRESS  0x27            /**< Device address. Standard is often 0x27 */
#define LCD_1602_SCREEN_CHAR_WIDTH 16   /**< Max character width of the screen */
#define LCD_1602_MAX_ROWS 2             /**< Max rows available on the screen */
/**@} */


/**
 * @brief Enum for returning the result of writing to the LCD.
 */
typedef enum LCD_WRITE_STATUS{
    LCD_WRITE_FINISHED,
    LCD_WRITE_NOT_FINISHED,
    LCD_WRITE_INTERRUPTED,
    LCD_TOO_LONG_STRING
} LCD_WRITE_STATUS;

/**
 * @brief External functions for LCD 1602 API.
 * @defgroup external_functions External Functions
 * @ingroup lcd_1602_api
 * @{
 */
/**
 * @brief Writes out data to the LCD.
 * 
 * @param handle The device handle used to writing on the i2c bus
 * @param str The string to be written to the LCD
 * 
 * @note Clears the screen first. '\n' moves to the next row and text wraps automatically.
 *
 * @return LCD_WRITE_FINISHED for successful. LCD_WRITE_INTERRUPTED if string is too long for
 * the screen or if communication on the i2c bus failed.
 */
LCD_WRITE_STATUS lcd_1602_send_string(i2c_master_dev_handle_t handle, const char *str);

/**
 * @brief initializes the LCD 1602 correctly according to the datasheet.
 *
 * Always turns the backlight on, even if lcd_1602_backlight_off() was called before.
 * 
 * @param handle The device handle use to writing on the i2c bus
 * 
 * @return 0 for success or 1 for fail.
 */
uint8_t lcd_1602_init(i2c_master_dev_handle_t handle);

/**
 * @brief Writes a single character at the current cursor position.
 *
 * @param handle The device handle used to writing on the i2c bus
 * @param c The character to write
 *
 * @return 0 for success or 1 for fail.
 */
uint8_t lcd_1602_send_char(i2c_master_dev_handle_t handle, char c);

/**
 * @brief Clears the screen and moves the cursor to the first row, first column.
 *
 * @param handle The device handle used to writing on the i2c bus
 *
 * @return 0 for success or 1 for fail.
 */
uint8_t lcd_1602_clear_screen(i2c_master_dev_handle_t handle);

/**
 * @brief Writes a string to the top row without changing the bottom row.
 *
 * Any remaining space on the row is filled with blanks, so old text on the
 * row is erased. Writing stops at '\0', '\n' or the end of the row.
 *
 * @param handle The device handle used to writing on the i2c bus
 * @param str The string to write
 *
 * @return LCD_WRITE_FINISHED if the whole string fit, LCD_TOO_LONG_STRING if it was
 * truncated, LCD_WRITE_INTERRUPTED if communication on the i2c bus failed.
 */
LCD_WRITE_STATUS lcd_1602_write_top(i2c_master_dev_handle_t handle, const char *str);

/**
 * @brief Writes a string to the bottom row without changing the top row.
 *
 * Any remaining space on the row is filled with blanks, so old text on the
 * row is erased. Writing stops at '\0', '\n' or the end of the row.
 *
 * @param handle The device handle used to writing on the i2c bus
 * @param str The string to write
 *
 * @return LCD_WRITE_FINISHED if the whole string fit, LCD_TOO_LONG_STRING if it was
 * truncated, LCD_WRITE_INTERRUPTED if communication on the i2c bus failed.
 */
LCD_WRITE_STATUS lcd_1602_write_bottom(i2c_master_dev_handle_t handle, const char *str);

/**
 * @brief Turns off the backlight. The screen content is kept.
 *
 * The backlight stays off for all later writes until lcd_1602_backlight_on() is called.
 *
 * @param handle The device handle used to writing on the i2c bus
 *
 * @return 0 for success or 1 for fail.
 */
uint8_t lcd_1602_backlight_off(i2c_master_dev_handle_t handle);

/**
 * @brief Turns on the backlight. The backlight is on by default.
 *
 * @param handle The device handle used to writing on the i2c bus
 *
 * @return 0 for success or 1 for fail.
 */
uint8_t lcd_1602_backlight_on(i2c_master_dev_handle_t handle);
/**@} */

#ifdef __cplusplus
}
#endif

#endif

/**@} */