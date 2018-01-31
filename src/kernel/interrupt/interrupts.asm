.extern isr_handler, exc_handler, IRQ_BASE

.set IRQ_BASE, 0x20
.set KCODE_SEG_OFFSET, 0x08
.set KDATA_SEG_OFFSET, 0x10
.set UCODE_SEG_OFFSET, 0x1B
.set UDATA_SEG_OFFSET, 0x23

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
    movw	$KDATA_SEG_OFFSET, %ax
    movw	%ax, %ds
#    movw	%ax, %es
#    movw 	%ax, %fs
#    movw	%ax, %gs
#    movw 	%ax, %ss

.endm

.macro RESTORE_REGS

    popl %eax
#    movw %ax, %ds
#    movw %ax, %es
#    movw %ax, %fs
#    movw %ax, %gs
#    movw %ax, %ss

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
    pushl 0 # Push a 4bytes as it is removed at the end of exception_handler()
    movb $\n, (exception_number)
    jmp exception_handler
.endm

.macro exc_handler_errc n
.global exc_handler_errc_\n
exc_handler_errc_\n:
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

# exc_handler_errc exception with error code
exc_handler         0x00
exc_handler         0x01
exc_handler         0x02
exc_handler         0x03
exc_handler         0x04
exc_handler         0x05
exc_handler         0x06
exc_handler         0x07
exc_handler         0x08
exc_handler         0x09
exc_handler_errc    0x0A
exc_handler_errc    0x0B
exc_handler_errc    0x0C
exc_handler_errc    0x0D
exc_handler_errc    0x0E
exc_handler         0x0F
exc_handler         0x10
exc_handler         0x11
exc_handler         0x12
exc_handler         0x13
exc_handler         0x14
exc_handler         0x15

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

    # Jump to kernel code segment
#    jmp	$KCODE_SEG_OFFSET, $jmp_kCodeSeg_exc
#    jmp_kCodeSeg_exc:

    pushl %esp                  # push stack pointer
    pushl (exception_number)    # push the exception nber as paramter
    call exc_handler            # call the isr, return value in eax register
    add $8, %esp                # Remove the parameters

    RESTORE_REGS                # Restore the registers

    # Jump to previous code segment
#    pushl %eax                   # Save register eax
#    movl %ds, %eax

    # Jump if eax != UDATA_SEG_OFFSET
#    cmp $(UDATA_SEG_OFFSET), %eax
#    jne restore_codeSeg_exc

#    jmp $UCODE_SEG_OFFSET, $restore_codeSeg_int

#restore_codeSeg_exc:
#    popl %eax                   # restore register eax

    add $4, %esp                # Remove error code

    sti                         # Enable interrupt
    iret                        # interrupt return

# ------------------------------------------------------------------------------
# Call the interrupt service routine
# 1. Save the registers
# 2. Call the function isr_handler
# 3. Restore the registers
interrupt_handler:

    SAVE_REGS                   # Save the registers

    # Jump to kernel code segment
#    jmp	$KCODE_SEG_OFFSET, $jmp_kCodeSeg_int
#    jmp_kCodeSeg_int:

    pushl %esp                  # push stack pointer
    pushl (interrupt_number)    # push the interrupt nber as paramter
    call isr_handler            # call the isr
    add $8, %esp                # Remove the parameters
    mov %eax, %esp              # switch the stack

    RESTORE_REGS                # Restore the registers

    # Jump to previous code segment
#    pushl %eax                   # Save register eax
#    movl %ds, %eax

    # Jump if eax != UDATA_SEG_OFFSET
#    cmp $(UDATA_SEG_OFFSET), %eax
#    jne restore_codeSeg_int

#    jmp $UCODE_SEG_OFFSET, $restore_codeSeg_int

#restore_codeSeg_int:
#    popl %eax                   # restore register eax

    add $4, %esp                # Remove error code

    sti                         # Enable interrupt
    iret                        # interrupt return

# ------------------------------------------------------------------------------
# Function call when the interruption is ignored
.global interrupt_ignored
interrupt_ignored:

    iret            # interrupt return

# ------------------------------------------------------------------------------

.data
    interrupt_number:  .byte 0
    exception_number:  .byte 0
