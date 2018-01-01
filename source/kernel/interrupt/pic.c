/*
*   Programmable Interrupt Controller
*/

#include <kernel/interrupt/pic.h>

void pic_init()
{
    // Initialization of ICW1
    io_write8(PORT_PIC_MASTER_COMMAND, PIC_ICW1 | PIC_ICW1_ICW4);
    io_write8(PORT_PIC_SLAVE_COMMAND,  PIC_ICW1 | PIC_ICW1_ICW4);

    // Initialization of ICW2, Master and Slave PIC vector offset
    io_write8(PORT_PIC_MASTER_DATA, PIC_ICW2_MASTER);
    io_write8(PORT_PIC_SLAVE_DATA,  PIC_ICW2_SLAVE);

    // Initialization of ICW3
    io_write8(PORT_PIC_MASTER_DATA, PIC_ICW3_MASTER);
    io_write8(PORT_PIC_SLAVE_DATA,  PIC_ICW3_SLAVE);

    // Initialization of ICW4
    io_write8(PORT_PIC_MASTER_DATA, PIC_ICW4_8086);
    io_write8(PORT_PIC_SLAVE_DATA,  PIC_ICW4_8086);

    // mask interrupts
    io_write8(PORT_PIC_MASTER_DATA, PIC_OCW1_MASTER);
    io_write8(PORT_PIC_SLAVE_DATA,  PIC_OCW1_SLAVE);
}
