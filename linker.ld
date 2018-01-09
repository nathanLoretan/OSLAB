ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

/* The kernel realocated to 3GB + 1MB(for the GRUB and MBR) */
/* Each section is aligned to the page size 4KB with ALIGN(0x1000) */

SECTIONS
{
  /*. = 0xC0100000;*/
  . = 0x00100000;

   /* Text sections */
  .text ALIGN (0x1000) : /*AT(ADDR(.text)-0xC0000000)*/
  {
      *(.multiboot) /* Need to run with GRUB */
      *(.text)
  }

  /* Read-only data sections */
  .rodata ALIGN (0x1000) : /*AT(ADDR(.rodata)-0xC0000000)*/
  {
      *(.rodata*)
  }

  /* Data sections */
  .data ALIGN (0x1000) : /*AT(ADDR(.data)-0xC0000000)*/
  {
      *(.data)
  }

  /* Common and bss sections */
  .bss ALIGN (0x1000) : /*AT(ADDR(.bss)-0xC0000000)*/
  {
      *(COMMON)
      *(.bss)
  }
}
