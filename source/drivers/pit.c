/*
*   Programmable Interval Timer
*/

#include <drivers/pit.h>

void pit_set_timer(uint32_t us)
{
    if(us <= PIT_MIN_INTERVAL) {
        us = PIT_MIN_INTERVAL;
    }
    else if(us >= PIT_MAX_INTERVAL) {
        us = PIT_MAX_INTERVAL;
    }

    uint16_t divisor = (uint16_t) (PIT_CLK * ((double)us / 1000000.0));
    io_write8 (PORT_PIT_CH0,  divisor & 0xFF);
    io_write8 (PORT_PIT_CH0, (divisor >> 8) & 0xFF);
}
