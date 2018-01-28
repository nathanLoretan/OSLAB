/*
*   Global Descriptor table
* - How many entries ?
*/

#ifndef __KERNEL_GDT_H__
#define __KERNEL_GDT_H__

#include <common/types.h>
#include <kernel/memory/memory_layout.h>

// Access Byte
#define TYPE_PR     0x80    // Present
#define TYPE_RING0  0x00    // Privilege Kernel
#define TYPE_RING3  0x60    // Privilege user
#define TYPE_EX     0x08    // Executable: 1
#define TYPE_DC     0x04    // Direction bit, Grow up: 0, Grow down:1
#define TYPE_RW     0x02    // Read/Write

// KCODE_SEG: Kernel Code segment
// KDATA_SEG: Kernel Data segment
// UCODE_SEG: User Code segment
// UDATA_SEG: User Data segment
#define KCODE_SEG   (TYPE_PR | (1 << 4) | TYPE_RING0 | TYPE_EX | TYPE_RW)
#define KDATA_SEG   (TYPE_PR | (1 << 4) | TYPE_RING0 | TYPE_RW)
#define UCODE_SEG   (TYPE_PR | (1 << 4) | TYPE_RING3 | TYPE_EX | TYPE_RW)
#define UDATA_SEG   (TYPE_PR | (1 << 4) | TYPE_RING3 | TYPE_RW)

#define SEGMENT_DESCRIPTOR_FLAG_GR   0x8
#define SEGMENT_DESCRIPTOR_FLAG_SZ   0x4

/* Segment Selector */
struct seg_select {
    uint8_t  privilege  :2;     // Privilege level of the segment
    uint8_t  table      :1;     // 0 GDT, 1 current LDT
    uint16_t index      :13;
}__attribute__((packed));

/* Segment Descriptor */
struct seg_desc {
    uint16_t limit_1;       // limit bits 0:15
    uint16_t base_1;        // linear addr bits 0:15
    uint8_t  base_2;        // linear addr bits 16:23
    uint8_t  type;          // Access Byte
    uint8_t  limit_2    :4; // limit bits 16:19
    uint8_t  flags      :4;
    uint8_t  base_3;        // linear addr bits 24:31
}__attribute__((packed));

typedef struct seg_desc seg_desc_t;
typedef struct seg_select seg_select_t;

void gdt_init();
void gdt_seg_init(seg_desc_t* seg, uint8_t type, uint32_t limit, uint32_t base);
seg_select_t gdt_kData_segSelect();
seg_select_t gdt_kCode_segSelect();
seg_select_t gdt_uData_segSelect();
seg_select_t gdt_uCode_segSelect();

#endif // __KERNEL_GDT_H__
