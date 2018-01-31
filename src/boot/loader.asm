.global loader
.global _loader
.global pageDirectory
.global kPageTable
.extern kmain

.set MAGIC,     0x1BADB002
.set FLAGS,     (1<<0 | 1<<1)
.set CHECKSUM,  -(MAGIC + FLAGS)

.set KERNEL_BASE, 0xC0000000
.set KERNEL_PD_INDEX, (KERNEL_BASE >> 22) # Kernel, page directory index

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.macro INIT_KERNEL_PAGING

    movl $(kPageTable - KERNEL_BASE), %edi
    movl $0,    %esi    # First address to map
    movl $1024, %ecx    # Number of page to map

while:
    # Jumb if esi >= (KERNEL_STOP - KERNEL_BASE)
    cmpl $(_KERNEL_STOP - KERNEL_BASE), %esi
    jge end

    movl %esi,   %edx
    orl  $0x003, %edx   # Set entry as present and Read/Write
    movl %edx,  (%edi)

    addl $4096, %esi
    addl $4, %edi
    loop while

end:
    # Map the page table to page directory index 0 (virtual addr 0x00000000)
    movl $(kPageTable - KERNEL_BASE + 0x003), (pageDirectory - KERNEL_BASE)

    # Map the page table to page directory index 300 (virtual addr 0xC0000000)
    movl $(kPageTable - KERNEL_BASE + 0x003), (pageDirectory - KERNEL_BASE + KERNEL_PD_INDEX * 4)

.endm

.section .text
loader:

    INIT_KERNEL_PAGING

    # Indicate the address of the page directory
    movl $(pageDirectory - KERNEL_BASE), %ecx
	movl %ecx, %cr3

    # Enable paging
    movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	# Jump to higher half with an absolute jump.
	lea _start, %ecx
	jmp *%ecx

_start:

    # Remove the first entry of the page directory
    movl $0, pageDirectory

    # Force a TLB flush
    movl %cr3, %ecx
    movl %ecx, %cr3

    mov  $kernel_space, %esp
    push %eax
    push %ebx

    call  kmain # call kernel

.section .data
.align 4096
pageDirectory:
    .space 4096, 0
kPageTable:
    .space 4096, 0

.section .bss
.section .heap
.section .stack

# The kernel space start after the heap and the stack memory emplacement
kernel_space:
