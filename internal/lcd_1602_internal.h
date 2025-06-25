/**
 *
 * @file:       lcd_1602_registers.h
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:  Internal implementation of the LCD1602
 * @addtogroup lcd_1602_internal LCD1602 Internal
 * @{
 -------------------------------------------------------------------------------------------------*/

#ifndef LCD_1602_REGISTERS_H_
#define LCD_1602_REGISTERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/
#include "../lcd_1602.h"
#include <stdint.h>
#include <stdlib.h>

/* Exported defines ------------------------------------------------------------------------------*/
/** @defgroup lcd_1602_defines Internal Defines
 * @ingroup lcd_1602_internal
 *  @{ */
#define LCD_1602_CLEAR_SCREEN           0x01        /**< Clear screen and sets cursor position to 0*/
#define LCD_1602_RESET_CURSOR_POS       0x02        /**< Resets the cursor position without changing the content*/

#define LCD_1602_INPUT_SET_MASK         0x07        /**< Bitmask for configuring input settings*/
#define LCD_1602_INPUT_SET              0x04        /**< Flag for configuring input settings */
#define LCD_1602_INCREMENT_MODE         0x02        /**< Increments the DDRAM address by 1 */
#define LCD_1602_DECREMENT_MODE         0x00        /**< Dectrements the DDRAM address by 1 */
#define LCD_1602_CURSOR_MOVE_ON_WRITE   0x01        /**< Move cursor when byte is written */
#define LCD_1602_CURSOR_N_MOVE          0x00        /**< Cursor does not move when byte is written */

#define LCD_1602_DISPLAY_SWITCH_MASK    0x0F        /**< Bitmask for configuring display settings */
#define LCD_1602_DISPLAY_SWITCH_FLAG    0x08        /**< Flag for configuring display settings */
#define LCD_1602_DISPLAY_ON             0x04        /**< Display on */
#define LCD_1602_DISPLAY_OFF            0x00        /**< Display off */
#define LCD_1602_CURSOR_ON              0x02        /**< Show cursor */
#define LCD_1602_CURSOR_OFF             0x00        /**< Hide cursor */
#define LCD_1602_BLINK_DISPLAY          0x01        /**< Blink the display */
#define LCD_1602_N_BLINK_DISPLAY        0x00        /**< Stop blinking the display */

#define LCD_1602_SHIFT_BITMASK          0x1C        /**< Bitmask for shifting cursor or screen */
#define LCD_1602_SHIFT_FLAG             0x10        /**< Flag for shifting cursor or screen */
#define LCD_1602_CURSOR_MOVE            0x00        /**< Allows the cursor to move */
#define LCD_1602_DISPLAY_SHIFT          0x08        /**< Shifts the display, cursor follows */
#define LCD_1602_SHIFT_LEFT             0x00        /**< Shift left */
#define LCD_1602_SHIFT_RIGHT            0x04        /**< Shift right */

#define LCD_1602_FUNCTION_SET_MASK      0x3C        /**< Bitmask for configuring the function of the screen */
#define LCD_1602_FUNCTION_SET_FLAG      0x20        /**< Flag for configuring the function of the screen */
#define LCD_1602_DATA_LEN_8_BIT         0x10        /**< Set datalength to be 8-bit */
#define LCD_1602_DATA_LEN_4_BIT         0x00        /**< Set datalength to be 4-bit */
#define LCD_1602_2_ROWS                 0x08        /**< Sets the display to 2-rows */
#define LCD_1602_1_ROW                  0x00        /**< Sets the display to single row */
#define LCD_1602_FONT_5X10              0x04        /**< Sets font to 5x10 */
#define LCD_1602_FONT_5X7               0x00        /**< Sets font to 5x7 */

#define LCD_1602_BACKLIGHT              0x08
#define LCD_1602_ENABLE                 0x04
#define LCD_1602_WRITE_DATA             0x00
#define LCD_1602_RS                     0x01

/** @} template_exported_defines */

/* Exported macros -------------------------------------------------------------------------------*/
/** @addtogroup template_exported_macros Exported macros
 *  @{ */
/**
 * @brief Macro for setting the correct movement of the cursor. Use together with:
 * - LCD_1602_INCREMENT_MODE
 * - LCD_1602_DECREMENT_MODE
 * - LCD_1602_CURSOR_MOVE_ON_WRITE
 * - LCD_1602_CURSOR_N_MOVE
 * 
 * @param dir the direction to move the cursor. LCD_1602_INCREMENT_MODE for forward | LCD_1602_DECREMENT_MODE for backwards
 * @param mov if the cursor should move when bytes are written. LCD_1602_CURSOR_MOVE_ON_WRITE or LCD_1602_CURSOR_N_MOVE
 * 
 * @note the macro do not change the register, but returns a bitfield to be written to a register.
 */
#define LCD_1602_CONFIG_INPUT_SET(dir, mov) ((LCD_1602_INPUT_SET | dir | mov) & LCD_1602_INPUT_SET_MASK)

/**
 * @brief Macro for configuring the display settings. Use together with;
 * - LCD_1602_DISPLAY_ON
 * - LCD_1602_DISPLAY_OFF
 * - LCD_1602_CURSOR_ON
 * - LCD_1602_CURSOR_OFF
 * - LCD_1602_BLINK_DISPLAY
 * - LCD_1602_N_BLINK_DISPLAY
 * 
 * @param disp if the display should be turned on or off. LCD_1602_DISPLAY_ON | LCD_1602_DISPLAY_OFF
 * @param curs if the cursor should be turned on or off. LCD_1602_CURSOR_ON | LCD_1602_CURSOR_OFF
 * @param blink if the screen should blink or not. LCD_1602_BLINK_DISPLAY | LCD_1602_N_BLINK_DISPLAY
 * 
 * @note the macro do not change the register, but returns a bitfield to be written to a register.
 */
#define LCD_1602_CONFIG_DISPLAY_SWITCH(disp, curs, blink)   ((LCD_1602_DISPLAY_SWITCH_FLAG | disp | curs | blink) & LCD_1602_DISPLAY_SWITCH_MASK)

/**
 * @brief Macro for moving the cursor or the display. User together with:
 * - LCD_1602_CURSOR_MOVE
 * - LCD_1602_DISPLAY_SHIFT
 * - LCD_1602_SHIFT_LEFT
 * - LCD_1602_SHIFT_RIGHT
 * 
 * @param ele the element to be moved. LCD_1602_CURSOR_MOVE for cursor | LCD_1602_DISPLAY_SHIFT for whole display
 * @param dir the direction to move. LCD_1602_SHIFT_LEFT | LCD_1602_SHIFT_RIGHT
 * 
 * @note the macro do not change the register, but returns a bitfield to be written to a register.
 */
#define LCD_1602_SHIFT(ele, dir)    ((LCD_1602_SHIFT_FLAG | ele | dir) & LCD_1602_SHIFT_BITMASK)

/**
 * @brief Macro for configuring the lcd screens function. Use together with:
 * - LCD_1602_DATA_LEN_8_BIT
 * - LCD_1602_DATA_LEN_4_BIT
 * - LCD_1602_2_ROWS
 * - LCD_1602_1_ROW
 * - LCD_1602_FONT_5X10
 * - LCD_1602_FONT_5X7
 * 
 * @param dl signifies the data-length of read/write. LCD_1602_DATA_LEN_8_BIT | LCD_1602_DATA_LEN_4_BIT
 * @param r sets the amount of rows used. LCD_1602_2_ROWS | LCD_1602_1_ROW
 * @param f sets the font to be used. LCD_1602_FONT_5X10 | LCD_1602_FONT_5X7
 * 
 * @note the macro do not change the register, but returns a bitfield to be written to a register.
 */
#define LCD_1602_FUNCTION_SET(dl, r, f) ((LCD_1602_FUNCTION_SET_FLAG | dl | r | f) & LCD_1602_FUNCTION_SET_MASK)

/** @} template_exported_macros */


/* Exported type definitions ---------------------------------------------------------------------*/
/** @addtogroup template_exported_typedefs Exported type definitions
 *  @{ */
/** @} template_exported_typedefs */

/* Exported function prototypes ------------------------------------------------------------------*/
/** @addtogroup template_exported_functions Exported functions
 *  @{ */
/** @} template_exported_functions */

#ifdef __cplusplus
}
#endif

#endif  // SRC_TEMPLATE_TEMPLATE_H_
/** @} template */
