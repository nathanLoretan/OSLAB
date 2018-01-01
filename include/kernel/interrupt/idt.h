/*
*   Interrupt Descriptor table
*/

#ifndef __KERNEL_IDT_H__
#define __KERNEL_IDT_H__

#include <common/types.h>
#include <kernel/gdt.h>

#define IDT_SIZE   256

#define IDT_TASK_GATE_32    0x5
#define IDT_TRAP_GATE_16    0x7
#define IDT_TRAP_GATE_32    0xF
#define IDT_INT_GATE_16     0x6
#define IDT_INT_GATE_32     0xE

#define IDT_PRIVILEGE_KERNEL    0
#define IDT_PRIVILEGE_USER      3

typedef void (*isr_t)();

/* Segment Descriptor */
struct idt_entry_desc {
    uint16_t handler_1;     // handler address bits 0:15
    uint16_t selector;
    uint8_t  reserved;
    uint8_t  type       :4;
    uint8_t  privilege  :1;
    uint8_t  storage    :2;
    uint8_t  present    :1;
    uint16_t handler_2;     // handler address bits 16:31
}__attribute__((packed));

typedef struct idt_entry_desc idt_entry_desc_t;

void idt_init();
void idt_interrupt_init(uint8_t interrupt, uint16_t selector, isr_t handler,
                        uint8_t type, uint8_t  privilege);

#endif // __KERNEL_IDT_H__
