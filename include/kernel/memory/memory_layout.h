#ifndef __MEMORY_LAYOUT_H__
#define __MEMORY_LAYOUT_H__

extern uint32_t _HEAP_BASE;
extern uint32_t _KERNEL_STOP;

// Indicate the virtual address of the user and kernel space
#define KERNEL_BASE         0xC0000000
#define KERNEL_HEAP         ((uint32_t)&_HEAP_BASE)
// #define KERNEL_STACK        0xC0400000  // _stack_base
#define KERNEL_PD_INDEX     (KERNEL_BASE >> 22)

#define USER_BASE       0x00000000
// #define USER_HEAP       //
// #define USER_STACK      //
#define USER_PD_INDEX   (USER_BASE >> 22)

// UCODE = User Code Segment   (Ring 3)
// UDATA = User Data Segment   (Ring 3)
// KCODE = Kernel Code Segment (Ring 0)
// KDATA = Kernel Data Segment (Ring 0)
//
// The limit and the base of the segment are the same to provide separation but
// to avoid the segmentation to affect the addresses (Will be affected by paging)
#define UCODE_SEG_BASE      0x00000000
#define UDATA_SEG_BASE      0x00000000
#define KCODE_SEG_BASE      0x00000000
#define KDATA_SEG_BASE      0x00000000

#define UCODE_SEG_LIMIT     0xFFFFFFFF
#define UDATA_SEG_LIMIT     0xFFFFFFFF
#define KCODE_SEG_LIMIT     0xFFFFFFFF
#define KDATA_SEG_LIMIT     0xFFFFFFFF

// Indicate the physical address where to stare saving pages
#define PAGING_START    0x10000000//((uint32_t)&_KERNEL_STOP & 0xFFFFF000)

// Memory space for specific drivers
#define VIDEO_MEMORY   (0x000B8000 + 0xC0000000)

#endif // __MEMORY_LAYOUT_H__
