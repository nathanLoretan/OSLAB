/*
*   Global Descriptor table
* - How many entries ?
*/

#ifndef __KERNEL_GDT_H__
#define __KERNEL_GDT_H__

#include <common/types.h>

#define CODE_SEGMENT    0x9A
#define DATA_SEGMENT    0x92

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
    uint16_t base_1;        // base bits 0:15
    uint8_t  base_2;        // base bits 16:23
    uint8_t  type;
    uint8_t  limit_2    :4; // limit bits 16:19
    uint8_t  flags      :4;
    uint8_t  base_3;        // base bits 24:31
}__attribute__((packed));

typedef struct seg_desc seg_desc_t;
typedef struct seg_select seg_select_t;

void gdt_init();
void gdt_seg_init(seg_desc_t* seg, uint8_t type, uint32_t limit, uint32_t base);
seg_select_t gdt_data_seg_select();
seg_select_t gdt_code_seg_select();

//------------------------------------------------------------------------------
//
// /* Local Descriptor Table */
// struct ldt {
//     seg_desc_t code_seg_desc;
//     seg_desc_t data_seg_desc;
//     seg_desc_t heap_seg_desc;
//     seg_desc_t stack_seg_desc;
// }__attribute__((packed));
//
// /* Local Descriptor Table */
// struct ldtr {
//     seg_desc_t code_seg_desc;
//     seg_desc_t data_seg_desc;
//     seg_desc_t heap_seg_desc;
//     seg_desc_t stack_seg_desc;
// }__attribute__((packed));
//
// typedef struct ldt ldt_t;
// typedef struct ldtr ldtr_t;

/* Segment Register */
// struct seg_register {
//     uint32_t  cs;    // Code Segement
//     uint32_t  ss;    // Stack Segement
//     uint32_t  ds;    // Data Segment
//     uint32_t  es;    // Additional Data Segemnt for current executing task
//     uint32_t  fs;    // Additional Data Segemnt for current executing task
//     uint32_t  gs;    // Additional Data Segemnt for current executing task
// }__attribute__((packed));
//

// typedef struct seg_register seg_register_t;

/* System Descriptor Types */
// #define SYSTEM_DESC_LDT         // Local descriptor-table (LDT) segment descriptor
// #define SYSTEM_DESC_TSS         // Task-state segment (TSS) descriptor
// #define SYSTEM_DESC_CALL_GATE   // Call-gate descriptor
// #define SYSTEM_DESC_INT_GATE    // Interrupt-gate descriptor
// #define SYSTEM_DESC_TRAP_GATE   // Trap-gate descriptor
// #define SYSTEM_DESC_TASK_GATE   // Task-gate descriptor

/* TODO:
    1. possibility to add a ldt and tss segment to gdt
    2. enable paging with CR0
    3. get page fault exception with cr2
    4. use cr3 to
    5. set paging mode with cr4 (32-bit paging, PAE paging, IA-32e paging)
    6. How to construct a ldt table
*/

#endif // __KERNEL_GDT_H__
