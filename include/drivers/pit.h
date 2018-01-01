/*
*   Programmable Interval Timer
*/

#ifndef __KERNEL_PIT_H__
#define __KERNEL_PIT_H__

#include <common/types.h>
#include <drivers/io.h>
#include <syscalls/print.h>

#define PIT_CLK             1193182 //  1.193182 MHz
#define PIT_MIN_INTERVAL    1       // us (really 838ns)
#define PIT_MAX_INTERVAL    55000   // us (50ms)

// Select Channel
#define PIT_CMD_CH0         0x00 // Channel 0
#define PIT_CMD_CH1         0x01 // Channel 1
#define PIT_CMD_CH2         0x02 // Channel 2
#define PIT_CMD_READBACK    0x03 // Read-back command (8254 only)

// Access Mode
#define PIT_CMD_LCVC        0x00 // Latch count value command
#define PIT_CMD_LO          0x01 // Access mode: lobyte only
#define PIT_CMD_HI          0x02 // Access mode: hibyte only
#define PIT_CMD_HILO        0x03 // Access mode: lobyte/hibyte

// Operating Mode
#define PIT_CMD_MODE0       0x00  // Mode 0 (interrupt on terminal count)
#define PIT_CMD_MODE1       0x01  // Mode 1 (hardware re-triggerable one-shot)
#define PIT_CMD_MODE2       0x02  // Mode 2 (rate generator)
#define PIT_CMD_MODE3       0x03  // Mode 3 (square wave generator)
#define PIT_CMD_MODE4       0x04  // Mode 4 (software triggered strobe)
#define PIT_CMD_MODE5       0x05  // Mode 5 (hardware triggered strobe)

// BCD/Binary mode
#define PIT_CMD_BINARY      0x00 // 16-bit binary
#define PIT_CMD_BCD         0x01 // four-digit BCD

struct pit_cmd {
    uint8_t binary     :1;  // BCD/Binary mode
    uint8_t operating  :3;  // Operating Mode
    uint8_t access     :2;  // Access Mode
    uint8_t channel    :2;  // Channel
} __attribute__ ((packed));

typedef struct pit_cmd pit_cmd_t;

void pit_set_timer(uint32_t interval);

#endif  // __KERNEL_PIT_H__
