/*
*   Interrupt Manager
*/

#include <kernel/interrupt/interrupt_manager.h>

extern void interrupt_ignored();
extern void interrupt_0x00();
extern void interrupt_0x01();
extern void interrupt_0x02();
extern void interrupt_0x03();
extern void interrupt_0x04();
extern void interrupt_0x05();
extern void interrupt_0x06();
extern void interrupt_0x07();
extern void interrupt_0x08();
extern void interrupt_0x09();
extern void interrupt_0x0A();
extern void interrupt_0x0B();
extern void interrupt_0x0C();
extern void interrupt_0x0D();
extern void interrupt_0x0E();
extern void interrupt_0x0F();
extern void interrupt_0x10();
extern void interrupt_0x11();
extern void interrupt_0x12();
extern void interrupt_0x13();
extern void interrupt_0x14();
extern void interrupt_0x15();

extern void exc_handler_0x00(); // 0x00 	Division by zero
extern void exc_handler_0x01(); // 0x01 	Single-step interrupt
extern void exc_handler_0x02(); // 0x02 	NMI
extern void exc_handler_0x03(); // 0x03 	Breakpoint
extern void exc_handler_0x04(); // 0x04 	Overflow
extern void exc_handler_0x05(); // 0x05 	Bounds
extern void exc_handler_0x06(); // 0x06 	Invalid Opcode
extern void exc_handler_0x07(); // 0x07 	Coprocessor not available
extern void exc_handler_0x08(); // 0x08 	Double fault
extern void exc_handler_0x09(); // 0x09 	Coprocessor Segment Overrun
extern void exc_handler_0x0A(); // 0x0A 	Invalid Task State Segment
extern void exc_handler_0x0B(); // 0x0B 	Segment not present
extern void exc_handler_0x0C(); // 0x0C 	Stack Fault
extern void exc_handler_0x0D(); // 0x0D 	General protection fault
extern void exc_handler_0x0E(); // 0x0E 	Page fault
extern void exc_handler_0x0F(); // 0x0F 	reserved
extern void exc_handler_0x10(); // 0x10 	Math Fault
extern void exc_handler_0x11(); // 0x11 	Alignment Check
extern void exc_handler_0x12(); // 0x12 	Machine Check
extern void exc_handler_0x13(); // 0x13 	SIMD Floating-Point Exception
extern void exc_handler_0x14(); // 0x14 	Virtualization Exception
extern void exc_handler_0x15(); // 0x15 	Control Protection Exception

#define HANDLER_QUEUE_SIZE  256

static isr_t im_handlers[IDT_SIZE];
static uint8_t im_handler_queue[HANDLER_QUEUE_SIZE];
static uint8_t idx_add = 0;
static uint8_t idx_get = 0;

static void im_queue_add(uint8_t int_id)
{
    im_handler_queue[idx_add] = int_id;
    idx_add = (idx_add + 1) % HANDLER_QUEUE_SIZE;
}

volatile bool_t im_queue_isEmpty()
{
    if(idx_get == idx_add) {
        return (volatile bool_t) TRUE;
    }
    else {
        return (volatile bool_t) FALSE;
    }
}

isr_t im_queue_get()
{
    uint8_t int_id;

    if(idx_get == idx_add) {
        return NULL;
    }

    int_id = im_handler_queue[idx_get];
    idx_get = (idx_get + 1) % HANDLER_QUEUE_SIZE;

    return im_handlers[int_id];
}


uint32_t exc_handler(uint8_t exc_id, uint32_t esp)
{
    switch(exc_id)
    {
        // case 0x00: {   // Division by zero
        // } break;
        // case 0x01: {   // Single-step interrupt
        // } break;
        // case 0x02: {   // NMI
        // } break;
        // case 0x03: {   // Breakpoint
        // } break;
        // case 0x04: {   // Overflow
        // } break;
        // case 0x05: {   // Bounds
        // } break;
        // case 0x06: {   // Invalid Opcode
        // } break;
        // case 0x07: {   // Coprocessor not available
        // } break;
        // case 0x08: {   // Double fault
        // } break;
        // case 0x09: {   // Coprocessor Segment Overrun
        // } break;
        // case 0x0A: {   // Invalid Task State Segment
        // } break;
        // case 0x0B: {   // Segment not present
        // } break;
        // case 0x0C: {   // Stack Fault
        // } break;
        // case 0x0D: {   // General protection fault
        // } break;
        case 0x0E: {  // Page fault
            uint32_t cr2;
            asm volatile ("movl %%cr2, %0": :"r" (cr2));
            printf("Page Fault: ");
            printfHex32(cr2);
            printf("\n");
        } break;
        // case 0x0F: {   // reserved
        // } break;
        // case 0x10: {   // Math Fault
        // } break;
        // case 0x11: {   // Alignment Check
        // } break;
        // case 0x12: {   // Machine Check
        // } break;
        // case 0x13: {   // SIMD Floating-Point Exception
        // } break;
        // case 0x14: {   // Virtualization Exception
        // } break;
        // case 0x15: {   // Control Protection Exception
        // } break;

        default: {
            printf("EXCEPTION: ");
            printfHex(exc_id);
            printf("\n");
        } break;
    }

    //Acknowledge the Interrupt
    if(exc_id >= IRQ_BASE && exc_id < IRQ_BASE + IRQ_NBR_MASTER + IRQ_NBR_SLAVE)
    {
        io_write8(PORT_PIC_MASTER_COMMAND, 0x20);

        if (exc_id >= IRQ_BASE + IRQ_NBR_MASTER){
            io_write8(PORT_PIC_SLAVE_COMMAND,  0x20);
        }
    }

    return esp;
}

uint32_t isr_handler(uint8_t int_id, uint32_t esp)
{
    switch(int_id)
    {
        case IRQ_BASE + 0x00: {   // Programmable Interrupt Timer Interrupt
            esp = (uint32_t)schedule_run((context_t*)esp);
        } break;
        // case IRQ_BASE + 0x01: {   // Keyboard Interrupt
        // } break;
        // case IRQ_BASE + 0x02: {   // Cascade (used internally by the two PICs. never raised)
        // } break;
        // case IRQ_BASE + 0x03: {   // COM2 (if enabled)
        // } break;
        // case IRQ_BASE + 0x04: {   // COM1 (if enabled)
        // } break;
        // case IRQ_BASE + 0x05: {   // LPT2 (if enabled)
        // } break;
        // case IRQ_BASE + 0x06: {   // Floppy Disk
        // } break;
        // case IRQ_BASE + 0x07: {   // LPT1
        // } break;
        // case IRQ_BASE + 0x08: {   // CMOS real-time clock (if enabled)
        // } break;
        // case IRQ_BASE + 0x09: {   // Free for peripherals / legacy SCSI / NIC
        // } break;
        // case IRQ_BASE + 0x0A: {   // Free for peripherals / SCSI / NIC
        // } break;
        // case IRQ_BASE + 0x0B: {   // Free for peripherals / SCSI / NIC
        // } break;
        // case IRQ_BASE + 0x0C: {   // PS2 Mouse
        // } break;
        // case IRQ_BASE + 0x0D: {   // FPU / Coprocessor / Inter-processor
        // } break;
        // case IRQ_BASE + 0x0E: {   // Primary ATA Hard Disk
        // } break;
        // case IRQ_BASE + 0x0F: {   // Secondary ATA Hard Disk
        // } break;

        default: {
            printf("INTERRUPT: ");
            printfHex(int_id);
            printf("\n");
        } break;
    }

    im_queue_add(int_id);

    //Acknowledge the Interrupt
    if(int_id >= IRQ_BASE && int_id < IRQ_BASE + IRQ_NBR_MASTER + IRQ_NBR_SLAVE)
    {
        io_write8(PORT_PIC_MASTER_COMMAND, 0x20);

        if (int_id >= IRQ_BASE + IRQ_NBR_MASTER){
            io_write8(PORT_PIC_SLAVE_COMMAND,  0x20);
        }
    }

    return esp;
}

void im_subscribe(isr_t handler, uint8_t interrupt)
{
    im_handlers[interrupt] = handler;
}

void im_init()
{
    seg_select_t code_segment = gdt_code_seg_select();

    // Reset Interrupt Descriptor Table
    for(uint16_t i = 0; i < IDT_SIZE; i++) {
        idt_interrupt_init(i, *(uint16_t*)&code_segment, interrupt_ignored, IDT_INT_GATE_32, 0);
    }

    // Initialize the exceptions
    idt_interrupt_init(EXC_BASE + 0x00, *(uint16_t*)&code_segment, exc_handler_0x00, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x01, *(uint16_t*)&code_segment, exc_handler_0x01, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x02, *(uint16_t*)&code_segment, exc_handler_0x02, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x03, *(uint16_t*)&code_segment, exc_handler_0x03, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x04, *(uint16_t*)&code_segment, exc_handler_0x04, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x05, *(uint16_t*)&code_segment, exc_handler_0x05, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x06, *(uint16_t*)&code_segment, exc_handler_0x06, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x07, *(uint16_t*)&code_segment, exc_handler_0x07, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x08, *(uint16_t*)&code_segment, exc_handler_0x08, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x09, *(uint16_t*)&code_segment, exc_handler_0x09, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x0A, *(uint16_t*)&code_segment, exc_handler_0x0A, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x0B, *(uint16_t*)&code_segment, exc_handler_0x0B, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x0C, *(uint16_t*)&code_segment, exc_handler_0x0C, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x0D, *(uint16_t*)&code_segment, exc_handler_0x0D, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x0E, *(uint16_t*)&code_segment, exc_handler_0x0E, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x0F, *(uint16_t*)&code_segment, exc_handler_0x0F, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x10, *(uint16_t*)&code_segment, exc_handler_0x10, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x11, *(uint16_t*)&code_segment, exc_handler_0x11, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x12, *(uint16_t*)&code_segment, exc_handler_0x12, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x13, *(uint16_t*)&code_segment, exc_handler_0x13, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x14, *(uint16_t*)&code_segment, exc_handler_0x14, IDT_INT_GATE_32, 0);
    idt_interrupt_init(EXC_BASE + 0x15, *(uint16_t*)&code_segment, exc_handler_0x15, IDT_INT_GATE_32, 0);

    idt_interrupt_init(IRQ_BASE + 0x00, *(uint16_t*)&code_segment, interrupt_0x00, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x01, *(uint16_t*)&code_segment, interrupt_0x01, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x02, *(uint16_t*)&code_segment, interrupt_0x02, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x03, *(uint16_t*)&code_segment, interrupt_0x03, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x04, *(uint16_t*)&code_segment, interrupt_0x04, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x05, *(uint16_t*)&code_segment, interrupt_0x05, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x06, *(uint16_t*)&code_segment, interrupt_0x06, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x07, *(uint16_t*)&code_segment, interrupt_0x07, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x08, *(uint16_t*)&code_segment, interrupt_0x08, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x09, *(uint16_t*)&code_segment, interrupt_0x09, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x0A, *(uint16_t*)&code_segment, interrupt_0x0A, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x0B, *(uint16_t*)&code_segment, interrupt_0x0B, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x0C, *(uint16_t*)&code_segment, interrupt_0x0C, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x0D, *(uint16_t*)&code_segment, interrupt_0x0D, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x0E, *(uint16_t*)&code_segment, interrupt_0x0E, IDT_INT_GATE_32, 0);
    idt_interrupt_init(IRQ_BASE + 0x0F, *(uint16_t*)&code_segment, interrupt_0x0F, IDT_INT_GATE_32, 0);

    // Initialize the Programmable Interrupt Controller
    pic_init();

    // Initialize the Interrupt Descriptor Table
    idt_init();

    // Initialize the list of interrupt handler
    for(uint16_t i = 0; i < IDT_SIZE; i++) {
        im_handlers[i] = NULL;
    }
}

void im_enable_interrupt()
{
    asm("sti");
}

void im_disable_interrupt()
{
    asm("cli");
}
