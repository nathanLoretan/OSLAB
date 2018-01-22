/*
*   Interrupt Descriptor table
*/

#include <kernel/interrupt/idt.h>

/* Interrupt Descriptor Table */
idt_entry_desc_t idt[IDT_SIZE];

struct idtr_t {
    uint16_t size;
    uint32_t offset;
}__attribute__((packed));

void idt_init()
{
    // Load the IDT using the LIDT assembly instruction.
    struct idtr_t idt_descriptor;
    idt_descriptor.size    = IDT_SIZE * sizeof(idt_entry_desc_t) - 1;
    idt_descriptor.offset  = (uint32_t)idt;
    asm volatile("lidt %0": :"m" (idt_descriptor));
}

void idt_interrupt_init(uint8_t interrupt, uint16_t selector, isr_t handler,
                        uint8_t type, uint8_t  privilege)
{
    idt[interrupt].handler_1 =  (uint32_t)handler & 0xFFFF;
    idt[interrupt].handler_2 = ((uint32_t)handler >> 16) & 0xFFFF;
    idt[interrupt].selector  = selector;
    idt[interrupt].type      = type;
    idt[interrupt].privilege = privilege;
    idt[interrupt].present   = 1;
    idt[interrupt].reserved  = 0;

    if((type == IDT_TRAP_GATE_16) | (type == IDT_INT_GATE_16) |
       (type == IDT_TRAP_GATE_32) | (type == IDT_INT_GATE_32))
       idt[interrupt].storage = 1;
}