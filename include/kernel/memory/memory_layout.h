#ifndef __MEMORY_LAYOUT_H__
#define __MEMORY_LAYOUT_H__

extern uint32_t _kernel_stop;

#define KERNEL_BASE         0xC0000000
#define KERNEL_HEAP         0xC0200000
#define KERNEL_STACK        //
#define KERNEL_PD_INDEX     (KERNEL_BASE >> 22)

#define USER_BASE       0x00000000
#define USER_HEAP       //
#define USER_STACK      //
#define USER_PD_INDEX   (USER_BASE >> 22)

#define PAGING_START    0x01000000 // _kernel_stop

// UCODE = User Code Segment   (Ring 3)
// UDATA = User Data Segment   (Ring 3)
// KCODE = Kernel Code Segment (Ring 0)
// KDATA = Kernel Data Segment (Ring 0)
//
// The limit and the base of the segment are the same to provide separation but
// to avoid the segmentation to affect the addresses (Will be affected by paging)
#define UCODE_SEG_BASE      0x00000000//USER_BASE
#define UDATA_SEG_BASE      0x00000000//USER_BASE
#define KCODE_SEG_BASE      0x00000000//KERNEL_BASE
#define KDATA_SEG_BASE      0x00000000//KERNEL_BASE

#define UCODE_SEG_LIMIT     0xFFFFFFFF
#define UDATA_SEG_LIMIT     0xFFFFFFFF
#define KCODE_SEG_LIMIT     0xFFFFFFFF
#define KDATA_SEG_LIMIT     0xFFFFFFFF

// TODO: Put all the driver into a common virtual area
// Memory space for specific drivers
#define VIDEO_MEMORY   (0x000B8000 + 0xC0000000)
// #define SATA_BASE
// #define AHCI_BASE     0x10000000

#endif // __MEMORY_LAYOUT_H__
