.extern isr_handler, exc_handler, IRQ_BASE

.set DATA_SEG_OFFSET, 0x10
.set IRQ_BASE, 0x20

.section .text
.macro SAVE_REGS
    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    movl %cr3, %eax
    pushl %eax

    movw %ds, %ax
    pushl %eax

    # Load kernel data segment
    movw	$DATA_SEG_OFFSET, %ax
    movw	%ax, %ds
    movw	%ax, %es
    movw 	%ax, %fs
    movw	%ax, %gs
    movw 	%ax, %ss

.endm

.macro RESTORE_REGS

    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    popl %eax
    movl %eax, %cr3

    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp
.endm

# Define the Interrupts and Esceptions handler using macro ---------------------
.macro exc_handler n
.global exc_handler_\n
exc_handler_\n:
    cli     # Disable interrupt
    movb $\n, (exception_number)
    jmp exception_handler
.endm

.macro interrupt n
.global interrupt_\n
interrupt_\n:
    cli     # Disable interrupt
    movb $\n + IRQ_BASE, (interrupt_number)
    pushl $0
    jmp interrupt_handler
.endm

exc_handler 0x00
exc_handler 0x01
exc_handler 0x02
exc_handler 0x03
exc_handler 0x04
exc_handler 0x05
exc_handler 0x06
exc_handler 0x07
exc_handler 0x08
exc_handler 0x09
exc_handler 0x0A
exc_handler 0x0B
exc_handler 0x0C
exc_handler 0x0D
exc_handler 0x0E
exc_handler 0x0F
exc_handler 0x10
exc_handler 0x11
exc_handler 0x12
exc_handler 0x13
exc_handler 0x14
exc_handler 0x15

interrupt 0x00
interrupt 0x01
interrupt 0x02
interrupt 0x03
interrupt 0x04
interrupt 0x05
interrupt 0x06
interrupt 0x07
interrupt 0x08
interrupt 0x09
interrupt 0x0A
interrupt 0x0B
interrupt 0x0C
interrupt 0x0D
interrupt 0x0E
interrupt 0x0F
interrupt 0x31
interrupt 0x80

# ------------------------------------------------------------------------------
# Call the interrupt service routine
# 1. Save the registers
# 2. Call the function isr_handler
# 3. Restore the registers
exception_handler:

    SAVE_REGS                   # Save the registers

    pushl %esp                  # push stack pointer
    push (exception_number)     # push the interrupt nber as paramter
    call exc_handler            # call the isr, return value in eax register
    mov %eax, %esp              # switch the stack

    RESTORE_REGS                # Restore the registers

    add $8, %esp                # Remove the exception number and error code

    sti                         # Enable interrupt
    iret                        # interrupt return

# ------------------------------------------------------------------------------
# Call the interrupt service routine
# 1. Save the registers
# 2. Call the function isr_handler
# 3. Restore the registers
interrupt_handler:

    SAVE_REGS                   # Save the registers

    pushl %esp                  # push stack pointer
    push (interrupt_number)     # push the interrupt nber as paramter
    call isr_handler            # call the isr
    mov %eax, %esp              # switch the stack

    RESTORE_REGS                # Restore the registers

    add $4, %esp                # Remove the interrupt number

    sti                         # Enable interrupt
    iret                        # interrupt return

# ------------------------------------------------------------------------------
# Function call when the interruption is ignored
.global interrupt_ignored
interrupt_ignored:

    iret            # interrupt return

# ------------------------------------------------------------------------------

.data
    interrupt_number: .byte 0
    exception_number: .byte 0
