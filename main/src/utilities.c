#include "utilities.h"

void non_blocking_delay_ms(uint8_t delay_ms) {
    sleep_ms(delay_ms);
}

void non_blocking_delay_us(uint8_t delay_us) {
    sleep_us(delay_us);
}