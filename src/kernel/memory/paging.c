/*
*   Paging
*/

#include <kernel/memory/paging.h>

size_t page_ptr = 0;
size_t pd_ptr = 1;
pd_t* page_directories = NULL;

uint32_t paging_getPage()
{
    return (PAGING_BASE_ADDRESS + (PAGING_SIZE * page_ptr++));
}

static void pd_init(pd_t* pd)
{
    for(uint32_t i = 0; i < 1024; i++)
    {
        pd[i] = (pd_t) {
                            .present        = 0,
                            .read_write     = 1,
                            .privilege      = 0,
                            .write_through  = 0,
                            .cache_disable  = 0,
                            .accessed       = 0,
                            .size           = 0,
                            .ignored        = 0,
                            .available      = 0,
                            .pt_addr        = 0,
                        };
    }
}

static void pt_init(pt_t* pt)
{
    for(uint32_t i = 0; i < 1024; i++)
    {
        pt[i] = (pt_t) {
                            .present        = 0,
                            .read_write     = 1,
                            .privilege      = 0,
                            .write_through  = 0,
                            .cache_disable  = 0,
                            .accessed       = 0,
                            .dirty          = 0,
                            .global         = 0,
                            .available      = 0,
                            .phy_addr       = 0,
                            // .phy_addr       = i,

                            // Not used now because identity mapping -----------
                            // .phy_addr       = paging_getPage() >> 12,
                            // -------------------------------------------------
                        };
    }
}

pd_t* paging_getPageDirectories()
{
    return page_directories;
}

void paging_enable()
{
    uint32_t cr3 = (uint32_t) page_directories & 0xFFFFF000;
    asm volatile ("movl %%eax, %%cr3" :: "a" (cr3));
    asm volatile ("movl %cr0, %eax; orl $0x80000001, %eax; movl %eax, %cr0;");
}

void paging_disable()
{
    asm volatile ("movl %cr0, %eax; andl $0x7FFFFFFF, %eax; movl %eax, %cr0;");
}

pd_t* paging_getKernelDirectory()
{
    if(page_directories == NULL) {
        page_directories = (pd_t*) paging_getPage();
        pd_init(page_directories);
    }

    return &page_directories[0];
}

pt_t* paging_getKernelTable()
{
    pt_t* pt = (pt_t*) paging_getPage();

    // The physical address used here are from address 0x00000000 (GRUB, MBR),
    // 0x00100000 (base of the kernel), 0x003FF000

    for(uint32_t i = 0; i < 1024; i++)
    {
        pt[i] = (pt_t) {
                            .present        = 1,
                            .read_write     = 1,
                            .privilege      = 0,
                            .write_through  = 0,
                            .cache_disable  = 0,
                            .accessed       = 0,
                            .dirty          = 0,
                            .global         = 0,
                            .available      = 0,
                            .phy_addr       = i,
                        };
    }

    return pt;
}

pd_t* paging_getDirectory()
{
    if(page_directories == NULL) {
        page_directories = (pd_t*) paging_getPage();
        pd_init(page_directories);
    }

    // Avoid to return &page_directories[0] which is kernel Directory
    if(pd_ptr == 0) {
        pd_ptr++;
    }

    return &page_directories[pd_ptr++];
}

pt_t* paging_getTable()
{
    pt_t* pt = (pt_t*) paging_getPage();
    pt_init(pt);
    return pt;
}

uint32_t paging_alloc(pt_t* pt)
{

    pt->present     = 1;
    pt->read_write  = 1;

    // Not used if identity mapping --------------------------------------------
    pt->phy_addr    = paging_getPage() >> 12;
    // -------------------------------------------------------------------------

    return pt->phy_addr << 12;
}

void paging_free(pt_t* pt)
{

}

void paging_addTableToDirectory(pd_t* pd, pt_t* pt)
{
    // Identity mapping: ONLY FOR TO TEST---------------------------------------
    // uint32_t pd_indexes = 0;
    //
    // for(int i = 0; i < 1024; i++)
    // {
    //     if(pd == &page_directories[i]) {
    //         pd_indexes = i;
    //         break;
    //     }
    // }
    //
    // for(int i = 0; i < 1024; i++) {
    //     pt[i].phy_addr = i | (pd_indexes << 10);
    // }
    // -------------------------------------------------------------------------

    pd->present     = 1;
    pd->read_write  = 1;
    pd->pt_addr     = (uint32_t)pt >> 12;
}

uint32_t* paging_getVirtualAddr(uint32_t pd_index, uint32_t pt_index, uint32_t offset)
{
    return (uint32_t*)((pd_index << 22) | (pt_index << 12 & 0x3FF000) | (offset & 0xFFF));
}

uint32_t* paging_getPhysicalAddr(uint32_t* virtualAddr)
{
    uint32_t pd_index = (uint32_t)virtualAddr >> 22;
    uint32_t pt_index = (uint32_t)virtualAddr >> 12 & 0x03FF;
    uint32_t offset   = (uint32_t)virtualAddr & 0xFFF;

    pt_t* pt = (pt_t*) (page_directories[pd_index].pt_addr << 12);

    return (uint32_t*)((pt[pt_index].phy_addr << 12) + offset);
}

uint32_t* paging_getVirtualBaseAddr(pd_t* pd)
{
    uint32_t pd_indexes = 0;

    for(int i = 0; i < 1024; i++)
    {
        if(pd == &page_directories[i]) {
            pd_indexes = i;
            break;
        }
    }

    return (uint32_t*) (pd_indexes << 22);
}
