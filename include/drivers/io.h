/*
*   IO
*/

#ifndef __DRIVERS_IO_H__
#define __DRIVERS_IO_H__

#include <common/types.h>

// Programmable Interrupt Controller
#define PORT_PIC_SLAVE_DATA     0xA1
#define PORT_PIC_MASTER_DATA    0x21
#define PORT_PIC_SLAVE_CMD      0xA0
#define PORT_PIC_MASTER_CMD     0x20

// Programmable Interval Timer
#define PORT_PIT_CH0        0x40 // Channel 0 data port (read/write)
#define PORT_PIT_CH1        0x41 // Channel 1 data port (read/write)
#define PORT_PIT_CH2        0x42 // Channel 2 data port (read/write)
#define PORT_PIT_CMD        0x43 // Mode/Command register (write only)

#define PORT_PS2_DATA       0x60 // Read/Write 	Data Port
#define PORT_PS2_CMD        0x64 // Read Status Register / Write Command Register

#define PORT_PS2_DATA       0x60 // Read/Write 	Data Port
#define PORT_PS2_CMD        0x64 // Read Status Register / Write Command Register

// Peripheral Componenent Interconnect
#define PORT_PCI_DATA       0xCFC
#define PORT_PCI_CMD        0xCF8

uint8_t  io_read8 (uint16_t port_number);
uint16_t io_read16(uint16_t port_number);
uint32_t io_read32(uint16_t port_number);

void io_write8 (uint16_t port_number, uint8_t  data);
void io_write16(uint16_t port_number, uint16_t data);
void io_write32(uint16_t port_number, uint32_t data);
void io_write8Slow(uint16_t port_number, uint8_t data);

#endif  // __DRIVERS_IO_H__
