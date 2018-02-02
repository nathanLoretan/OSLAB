/*
*   Programmable Interrupt Controller
*/

#ifndef __KERNEL_PIC_H__
#define __KERNEL_PIC_H__

#include <common/types.h>
#include <drivers/io.h>
#include <kernel/memory/memory_layout.h>

#define EXC_BASE        0x00
#define IRQ_BASE        0x20
#define IRQ_NBR_MASTER  0x08
#define IRQ_NBR_SLAVE   0x08

#define EXC_DIVIDE_ZERO         EXC_BASE + 0x00 	// Division by zero
#define EXC_SINGLE_STEP_INT     EXC_BASE + 0x01 	// Single-step interrupt
#define EXC_NMI                 EXC_BASE + 0x02 	// NMI
#define EXC_BREAKPOINT          EXC_BASE + 0x03 	// Breakpoint
#define EXC_OVERFLOW            EXC_BASE + 0x04 	// Overflow
#define EXC_BOUNDS              EXC_BASE + 0x05 	// Bounds
#define EXC_INVALID_OPCODE      EXC_BASE + 0x06 	// Invalid Opcode
#define EXC_COPROSS_UNVAILABLE  EXC_BASE + 0x07 	// Coprocessor not available
#define EXC_DOUBLE_FAULT        EXC_BASE + 0x08 	// Double fault
#define EXC_COPROSS_SEG_OVERRUN EXC_BASE + 0x09 	// Coprocessor Segment Overrun
#define EXC_TASK_STATE_SEG      EXC_BASE + 0x0A 	// Invalid Task State Segment
#define EXC_SEG_NOT_PRESENT     EXC_BASE + 0x0B 	// Segment not present
#define EXC_STACK_FAULT         EXC_BASE + 0x0C 	// Stack Fault
#define EXC_GEN_PROTECT_FAULT   EXC_BASE + 0x0D 	// General protection fault
#define EXC_PAGE_FAULT          EXC_BASE + 0x0E 	// Page fault
#define EXC_RESERVED            EXC_BASE + 0x0F 	// reserved
#define EXC_MATH_FAULT          EXC_BASE + 0x10 	// Math Fault
#define EXC_ALIGNMENT_CHECK     EXC_BASE + 0x11 	// Alignment Check
#define EXC_MACHINE_CHECK       EXC_BASE + 0x12 	// Machine Check
#define EXC_SMID_FPE            EXC_BASE + 0x13 	// SIMD Floating-Point Exception
#define EXC_VIRTUAL_EXC         EXC_BASE + 0x14 	// Virtualization Exception
#define EXC_CONTROL_PROTECT     EXC_BASE + 0x15 	// Control Protection Exception

#define IRQ_TIMER           IRQ_BASE + 0x00 	// Programmable Interrupt Timer Interrupt
#define IRQ_KEYBOARD        IRQ_BASE + 0x01 	// Keyboard Interrupt
#define IRQ_CASCADE         IRQ_BASE + 0x02 	// Cascade (used internally by the two PICs. never raised)
#define IRQ_COM2            IRQ_BASE + 0x03 	// COM2 (if enabled)
#define IRQ_COM1            IRQ_BASE + 0x04 	// COM1 (if enabled)
#define IRQ_LPT2            IRQ_BASE + 0x05 	// LPT2 (if enabled)
#define IRQ_FLOPPY_DISK     IRQ_BASE + 0x06 	// Floppy Disk
#define IRQ_LPT1            IRQ_BASE + 0x07 	// LPT1
#define IRQ_RTC             IRQ_BASE + 0x08 	// CMOS real-time clock (if enabled)
#define IRQ_FREE_PERIPH1    IRQ_BASE + 0x09 	// Free for peripherals / legacy SCSI / NIC
#define IRQ_FREE_PERIPH2    IRQ_BASE + 0x0A 	// Free for peripherals / SCSI / NIC
#define IRQ_FREE_PERIPH3    IRQ_BASE + 0x0B 	// Free for peripherals / SCSI / NIC
#define IRQ_MOUSE           IRQ_BASE + 0x0C 	// PS2 Mouse
#define IRQ_FPU             IRQ_BASE + 0x0D 	// FPU / Coprocessor / Inter-processor
#define IRQ_PRIMARY_HD      IRQ_BASE + 0x0E 	// Primary ATA Hard Disk
#define IRQ_SECONDARY_HD    IRQ_BASE + 0x0F 	// Secondary ATA Hard Disk

#define IRQ_YIELD           IRQ_BASE + 0x50 	// Scheduler Yield

// Initialisation Command Word One
// D7 | D6 | D5 | D4 |  D3  |   D2 |  D1  |  D0
// A7 | A6 | A5 | A1 | LTIM |  ADI | SNGL | IC4
#define PIC_ICW1            0x10    // ICW1 base /!\ required
#define PIC_ICW1_ICW4       0x01    // with ICW4 (1) / without (0)
#define PIC_ICW1_SINGLE     0x02    // single mode (1) / cascade mode (0)
#define PIC_ICW1_ADDR       0x04    // ISR 4 bytes (1) / 8 bytes (0) apart
#define PIC_ICW1_LEVEL      0x08    // triggering by level (1) / edge (0)

// Initialisation Command Word Two
//  D7 |  D6 |  D5 |  D4 |  D3 |   D2 |  D1 |  D0
// A15 | A14 | A13 | A12 | A11 |  A10 |  A9 |  A8
#define PIC_ICW2_MASTER     IRQ_BASE                     // Master PIC vector OFFSET
#define PIC_ICW2_SLAVE      IRQ_BASE + IRQ_NBR_MASTER    // SLAVE PIC vector OFFSET

// Initialisation Command Word Three
//          D7 | D6 | D5 | D4 | D3 |  D2 |  D1 |  D0
// Master:  S7 | S6 | S5 | S4 | S3 |  S2 |  S1 |  S0
// Slave:    0 |  0 |  0 |  0 |  0 | ID3 | ID2 | ID1
#define PIC_ICW3_MASTER     0x04    // Sx (1) slave is present
#define PIC_ICW3_SLAVE      0x02    // IDx is slave ID, Slave 4 on IR4 has ICW3=04h (0000 0100)

// Initialisation Command Word Four
// D7 |	D6 | D5 |  D4  |  D3 |  D2 |  D1  | D0
//  0 |	 0 |  0 | SFNM | BUF | M/S | AEOI | Mode
#define PIC_ICW4_8086       0x01    // Mode 8086 (1) / 8085 (0)
#define PIC_ICW4_AUTO       0x02	// Auto End of Interrupt (1) / normal (0)
#define PIC_ICW4_M_S        0x04    // Master (1) / Slave (0)
#define PIC_ICW4_BUF        0x08	// Buffered Slave (M/S=0) / Master (M/S=1)
#define PIC_ICW4_SFNM       0x10	// Special fully nested

// Operational Command Word One
// D7 |	D6 | D5 | D4 | D3 | D2 | D1	| D0
// M7 |	M6 | M5 | M4 | M3 | M2 | M1 | M0
#define PIC_OCW1_MASTER     0x00    // IRx is masked by setting Mx to 1
#define PIC_OCW1_SLAVE      0x00	// IRx is masked by setting Mx to 1

void pic_init();

#endif // __KERNEL_PIC_H__
