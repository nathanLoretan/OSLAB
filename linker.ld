ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

/* Define the memory mapping of the Kernel :

        +--------------------+
        |     Kernel space   |
        +--------------------+
        |       Stack        |
        +--------------------+
        |       Heap         |
        +--------------------+
        |    BSS Segment     |
        +--------------------+
        |    Data Segment    |
        +--------------------+
        |   RO Data Segment  |
        +--------------------+
        |    Text Segment    |
        +--------------------+
        |       Other        |
        +--------------------+

In this linker script, the heap and the stack has they own memory space and
don't overlap.

Half higher kernel is used, therefore, the kernel virtual address is 3GB.

/!\ WARNING /!\
If the kernel is bigger than 4MB several page tables need to be define during
the kernel boot.
*/

_STACK_SIZE   = 1M; /* heap size  = 1MB */
_HEAP_SIZE    = 1M; /* Stack size = 1MB */

_KERNEL_BASE  = 0xC0000000; /* Start virtual address of the kernel section */
_HEAP_BASE    = 0xC0200000; /* Start virtual address of the kernel section */

SECTIONS
{
    /* Start OS Space at 1M. Before, the memory is used by BIOS, MBR, GRUB, IVT
    and specific memory space */
    . = (_KERNEL_BASE + 1M);

    _KERNEL_START = .;

    /* Text sections */
    .text ALIGN(0x1000) : AT(ADDR(.text) - _KERNEL_BASE)
    {
      *(.multiboot) /* Need to run with GRUB */
      *(.text)
    }

    /* Read-only data sections */
    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - _KERNEL_BASE)
    {
      *(.rodata*)
    }

    /* Data sections */
    .data ALIGN(0x1000) : AT(ADDR(.data) - _KERNEL_BASE)
    {
      *(.data)
    }

    /* Common and bss sections */
    .bss ALIGN(0x1000) : AT(ADDR(.bss) - _KERNEL_BASE)
    {
      *(COMMON)
      *(.bss)
    }

    . = _HEAP_BASE;

    /* Heap Section */
    .heap ALIGN(0x1000) : AT(ADDR(.heap) - _KERNEL_BASE)
    {
      _heap_start = .;
      . += _HEAP_SIZE;
      _heap_end = .;
    }

    /* Stack section */
    .stack ALIGN(0x1000) : AT(ADDR(.stack) - _KERNEL_BASE)
    {
      _stack_end = .;
      . += _STACK_SIZE;
      _stack_start = .;
    }

    _KERNEL_STOP = .;
    PROVIDE(_kernel_stop = .);
}
