#include <stdio.h>
#include "pico/stdlib.h"
#include "lcd_1602.h"

int main()
{
    stdio_init_all();

    sleep_ms(5000);

    pico_i2c_open();
    lcd_1602_init();

    while (true) {
        lcd_1602_send_string("Hello World");
        sleep_ms(1000);

        lcd_1602_send_string("Goodbye World");
        sleep_ms(1000);
    }
}
