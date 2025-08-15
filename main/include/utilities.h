#ifndef _PICO_W_UTILITIES_H
#define _PICO_W_UTILITIES_H

#include <stdio.h>
#include "pico/stdlib.h"

void non_blocking_delay_ms(uint8_t delay_ms);
void non_blocking_delay_us(uint8_t delay_us);

#endif