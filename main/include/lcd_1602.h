#ifndef LCD_1602_H_
#define LCD_1602_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/
#include "pico_i2c.h"

/**
 * @brief configurable defines for setting up the LCD1602
 * @defgroup config_defines Configurable Defines
 * @ingroup lcd_1602_api
 * @{
 */
#define DEVICE_ADDRESS  0x27            /**< Device address. Standars is often 0x27 */
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
    LCD_TOO_LONG_STRING,
    LCD_WRITE_FAILED_SEND
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
 * @return LCD_WRITE_FINISHED for successful. LCD_WRITE_INTERRUPTED if string is too long for screen.
 */
LCD_WRITE_STATUS lcd_1602_send_string(char *str);

/**
 * @brief initializes the LCD 1602 correctly according to the datasheet.
 * 
 * @param handle The device handle use to writing on the i2c bus
 * 
 * @return 0 for success or 1 for fail.
 */
uint8_t lcd_1602_init();
/**@} */

#ifdef __cplusplus
}
#endif

#endif