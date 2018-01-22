/*
*   Global Descriptor table
*/

#include <kernel/gdt.h>

/* Global Descriptor Table */
struct gdt {
    seg_desc_t null_seg_desc;
    seg_desc_t unused_seg_desc;
    seg_desc_t code_seg_desc;
    seg_desc_t data_seg_desc;
}__attribute__((packed));

struct gdtr {
    uint16_t size;
    uint32_t offset;
}__attribute__((packed));

typedef struct gdt gdt_t;
typedef struct gdtr gdtr_t;

gdt_t gdt;
uint8_t ldt_indexe_entry = 0;

void gdt_init()
{
    gdt_seg_init(&gdt.null_seg_desc, 0, 0, 0);
    gdt_seg_init(&gdt.unused_seg_desc, 0, 0, 0);
    gdt_seg_init(&gdt.data_seg_desc, DATA_SEGMENT, 0xFFFFFFFF, 0);
    gdt_seg_init(&gdt.code_seg_desc, CODE_SEGMENT, 0xFFFFFFFF, 0);

    // Load the GDT using the LGDT assembly instruction.
    gdtr_t gdt_descriptor;
    gdt_descriptor.size   = sizeof(gdt);
    gdt_descriptor.offset = (uint32_t) &gdt;
    asm volatile("lgdt (%0)": :"p" (&gdt_descriptor));
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

seg_select_t gdt_data_seg_select()
{
    seg_select_t seg_select  = {
        .privilege  = 0,
        .table      = 0,
        .index      = ((uint8_t*)&gdt.data_seg_desc - (uint8_t*)&gdt) >> 3,
    };

    return seg_select;
    // return (uint8_t*)&gdt.data_seg_desc - (uint8_t*)&gdt;
}

seg_select_t gdt_code_seg_select()
{
    seg_select_t seg_select = {
        .privilege  = 0,
        .table      = 0,
        .index      = ((uint8_t*)&gdt.code_seg_desc - (uint8_t*)&gdt) >> 3,
    };

    return seg_select;
    // return (uint8_t*)&gdt.code_seg_desc - (uint8_t*)&gdt;
}
