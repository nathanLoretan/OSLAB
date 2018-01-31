/*
*   Global Descriptor table
*/

#include <kernel/gdt.h>
#include <lib/stdio.h>

/* Global Descriptor Table */
struct gdt {
    seg_desc_t null_segDesc;
    seg_desc_t kCode_segDesc;
    seg_desc_t kData_segDesc;
    seg_desc_t uCode_segDesc;
    seg_desc_t uData_segDesc;
}__attribute__((packed));
typedef struct gdt  gdt_t;

/* Global Descriptor Table Register */
struct gdtr {
    uint16_t size;
    uint32_t offset;    // the linear address of the table
}__attribute__((packed));
typedef struct gdtr gdtr_t;

gdt_t  gdt;
gdtr_t gdtr;

extern void gdt_flush(uint32_t);

void gdt_init()
{
    // The segmentation is not used for virtual memory, all base addresses are 0.
    // However, it needs to be define with x86 architecture and for kernel
    // protection.

    gdt_seg_init(&gdt.null_segDesc,  0, 0, 0);
    gdt_seg_init(&gdt.kCode_segDesc, KCODE_SEG, KCODE_SEG_LIMIT, KDATA_SEG_BASE);
    gdt_seg_init(&gdt.kData_segDesc, KDATA_SEG, KDATA_SEG_LIMIT, KCODE_SEG_BASE);
    gdt_seg_init(&gdt.uCode_segDesc, UCODE_SEG, UCODE_SEG_LIMIT, UCODE_SEG_BASE);
    gdt_seg_init(&gdt.uData_segDesc, UDATA_SEG, UDATA_SEG_LIMIT, UDATA_SEG_BASE);

    // seg_select_t sel1 = gdt_kCode_segSelect();
    // seg_select_t sel2 = gdt_kData_segSelect();
    // seg_select_t sel3 = gdt_uCode_segSelect();
    // seg_select_t sel4 = gdt_uData_segSelect();
    //
    // printf("%32x\n", *(uint32_t*)&sel1);
    // printf("%32x\n", *(uint32_t*)&sel2);
    // printf("%32x\n", *(uint32_t*)&sel3);
    // printf("%32x\n", *(uint32_t*)&sel4);

    gdtr.size   = sizeof(gdt) - 1;
    gdtr.offset = (uint32_t) &gdt;

    // When writting in lgdt register, the segement must be reload
    gdt_flush((uint32_t)&gdtr);
}

void gdt_seg_init(seg_desc_t* seg, uint8_t type, uint32_t limit, uint32_t base)
{
    // Check the limit to make sure that it is encoded correctly
    if (limit > 65536)
    {
        // // If the limit is higher than 65536 all the 20bits of the limit must be
        // // set to 1.
        // limit = 0x0FFF;
        // seg->flags = SEGMENT_DESCRIPTOR_FLAG_GR | SEGMENT_DESCRIPTOR_FLAG_SZ;

        if((limit & 0xFFF) != 0xFFF)
            limit = (limit >> 12)-1;
        else
            limit = limit >> 12;

        seg->flags = 0xC;
    }
    else
    {
        // seg->flags = SEGMENT_DESCRIPTOR_FLAG_SZ;
        seg->flags = 0x4;
    }

    seg->type = type & 0xFF;

    seg->limit_1 =  limit & 0xFFFF;
    seg->limit_2 = (limit >> 16) & 0xF;

    seg->base_1 =  base & 0xFFFF;
    seg->base_2 = (base >> 16) & 0xFF;
    seg->base_3 = (base >> 24) & 0xFF;
}

seg_select_t gdt_kData_segSelect()
{
    seg_select_t seg_select  = {
        .privilege  = 0,
        .table      = 0,
        .index      = ((uint8_t*)&gdt.kData_segDesc - (uint8_t*)&gdt) >> 3,
    };

    return seg_select;
    // return (uint8_t*)&gdt.data_segDesc - (uint8_t*)&gdt;
}

seg_select_t gdt_kCode_segSelect()
{
    seg_select_t seg_select = {
        .privilege  = 0,
        .table      = 0,
        .index      = ((uint8_t*)&gdt.kCode_segDesc - (uint8_t*)&gdt) >> 3,
    };

    return seg_select;
    // return (uint8_t*)&gdt.code_segDesc - (uint8_t*)&gdt;
}

seg_select_t gdt_uData_segSelect()
{
    seg_select_t seg_select  = {
        .privilege  = 3,
        .table      = 0,
        .index      = ((uint8_t*)&gdt.uData_segDesc - (uint8_t*)&gdt) >> 3,
    };

    return seg_select;
    // return (uint8_t*)&gdt.data_segDesc - (uint8_t*)&gdt;
}

seg_select_t gdt_uCode_segSelect()
{
    seg_select_t seg_select = {
        .privilege  = 3,
        .table      = 0,
        .index      = ((uint8_t*)&gdt.uCode_segDesc - (uint8_t*)&gdt) >> 3,
    };

    return seg_select;
    // return (uint8_t*)&gdt.code_segDesc - (uint8_t*)&gdt;
}
