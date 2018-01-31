/*
*   Paging
*/

#include <kernel/memory/paging.h>

#define PAGING_SIZE  0x1000 // 4kB

size_t pd_uPtr = (USER_BASE >> 22);
size_t pd_kPtr = (KERNEL_BASE >> 22) + 1;

size_t page_uPtr = 0;

extern pd_t pageDirectory;  // /!\ Define in load.asm
extern pt_t kPageTable;     // /!\ Define in load.asm

static uint32_t paging_uGetPage()
{
    return (PAGING_START + (PAGING_SIZE * page_uPtr++));
}

void pd_uInit(pd_t* pd)
{
    // for(uint32_t i = 0; i < 1024; i++)
    // {
    //     pd[i] = (pd_t) {
    //                         .present        = 0,
    //                         .read_write     = 1,
    //                         .privilege      = 0,
    //                         .write_through  = 0,
    //                         .cache_disable  = 0,
    //                         .accessed       = 0,
    //                         .size           = 0,
    //                         .ignored        = 0,
    //                         .available      = 0,
    //                         .pt_addr        = 0,
    //                     };
    // }

    *pd = (pd_t) {
                    .present        = 0,
                    .read_write     = 0,
                    .privilege      = 1,    // Allow user-mode access
                    .write_through  = 0,
                    .cache_disable  = 0,
                    .accessed       = 0,
                    .reserved       = 0,
                    .size           = 0,
                    .ignored        = 0,
                    .available      = 0,
                    .pt_addr        = 0,
                 };
}

void pt_uInit(pt_t* pt)
{
    for(uint32_t i = 0; i < 1024; i++)
    {
        pt[i] = (pt_t) {
                            .present        = 0,
                            .read_write     = 0,
                            .privilege      = 1,    // Allow user-mode access
                            .write_through  = 0,
                            .cache_disable  = 0,
                            .accessed       = 0,
                            .dirty          = 0,
                            .reserved       = 0,
                            .global         = 0,
                            .available      = 0,
                            .phy_addr       = 0,
                            // .phy_addr       = i,

                            // Not used now because identity mapping -----------
                            // .phy_addr       = paging_uGetPage() >> 12,
                            // -------------------------------------------------
                        };
    }
}

pd_t* paging_getPageDirectoryTable()
{
    // Return the physical address of the page directory
    return (pd_t*)((uint32_t)&pageDirectory - KERNEL_BASE);
}

void paging_enable()
{
    // task_t* task = scheduler_getCurrentTask();
    //
    // if(task != NULL) {
    //     task->isPagingEnabled = TRUE;
    // }

    uint32_t cr3 = ((uint32_t) (&pageDirectory) & 0xFFFFF000) - KERNEL_BASE;
    asm volatile ("movl %%eax, %%cr3" :: "a" (cr3));
    asm volatile ("movl %cr0, %eax; orl $0x80000001, %eax; movl %eax, %cr0;");
}

void paging_disable()
{
    // task_t* task; = scheduler_getCurrentTask();
    //
    // if(task != NULL) {
    //     task->isPagingEnabled = FALSE;
    // }

    asm volatile ("movl %cr0, %eax; andl $0x7FFFFFFF, %eax; movl %eax, %cr0;");
}

// pd_t* paging_getKernelDirectory()
// {
//     // if(pageDirectory == NULL) {
//     //     pageDirectory = (pd_t*) paging_uGetPage();
//     //     pd_init(pageDirectory);
//     // }
//
//     // Return the address
//     return &pageDirectory[pd_kPtr++];
// }
//
// pt_t* paging_getKernelTable()
// {
//     pt_t* pt = (pt_t*) paging_uGetPage();
//
//     // The physical address used here are from address 0x00000000 (GRUB, MBR),
//     // 0x00100000 (base of the kernel), 0x003FF000
//
//     for(uint32_t i = 0; i < 1024; i++)
//     {
//         pt[i] = (pt_t) {
//                             .present        = 1,
//                             .read_write     = 1,
//                             .privilege      = 0,
//                             .write_through  = 0,
//                             .cache_disable  = 0,
//                             .accessed       = 0,
//                             .dirty          = 0,
//                             .global         = 0,
//                             .available      = 0,
//                             .phy_addr       = i,
//                         };
//     }
//
//     return pt;
// }

pd_t* paging_kGetDirectory()
{
    // if(pageDirectory == NULL) {
    //     pageDirectory = (pd_t*) paging_uGetPage();
    //     pd_init(pageDirectory);
    // }

    pd_t* pd = &pageDirectory;

    // Avoid to return the page index used to map the kernel at boot
    if(pd_kPtr == (KERNEL_BASE >> 22)) {
        pd_kPtr++;
    }

    return &pd[pd_kPtr++];
}

pd_t* paging_uGetDirectory()
{
    // if(pageDirectory == NULL) {
    //     pageDirectory = (pd_t*) paging_uGetPage();
    //     pd_init(pageDirectory);
    // }

    // Avoid to return &pageDirectory[0] which is kernel Directory
    // if(pd_uPtr == 0) {
    //     pd_uPtr++;
    // }

    pd_t* pd = &pageDirectory;
    return &pd[pd_uPtr++];
}

pd_t* paging_searchDirectory(uint32_t pd_index)
{
    pd_t* pd = (pd_t*) &pageDirectory;

    if(!pd[pd_index].present) {
        return NULL;
    }

    return &pd[pd_index];
}

pt_t* paging_searchTable(uint32_t pd_index)
{
    pd_t* pd = (pd_t*) &pageDirectory;

    if(!pd[pd_index].present || !pd[pd_index].pt_addr) {
        return NULL;
    }

    // The virtual address of the page table
    return (pt_t*) ((pd[pd_index].pt_addr << 12)  + KERNEL_BASE);
}

pt_t* paging_uGetTable()
{
    pt_t* pt = (pt_t*) paging_uGetPage();
    pt_uInit(pt);
    return pt;
}

uint32_t paging_alloc(pt_t* pt)
{
    pt->present     = 1;
    pt->read_write  = 1;
    pt->phy_addr    = paging_uGetPage() >> 12;

    // Force a TLB flush
    asm volatile ("movl %cr3, %ecx; movl %ecx, %cr3");

    return pt->phy_addr << 12;
}

void paging_free(pt_t* pt)
{
    // TODO
}

void paging_addTableToDirectory(pd_t* pd, pt_t* pt)
{
    // /!\ IMPORTANT /!\_
    // The address of the page table into the page directory is the PHYSICAL
    // ADDRESS!!!!!

    pd->present     = 1;
    pd->read_write  = 1;
    pd->pt_addr     = ((uint32_t)pt - KERNEL_BASE) >> 12;
}

uint32_t* paging_getVirtualAddr(uint32_t pd_index, uint32_t pt_index, uint32_t offset)
{
    return (uint32_t*)((pd_index << 22) | (pt_index << 12 & 0x3FF000) | (offset & 0xFFF));
}

uint32_t* paging_getPhysicalAddr(uint32_t* virtualAddr)
{
    uint32_t pd_index = (uint32_t)virtualAddr >> 22;
    uint32_t pt_index = (uint32_t)virtualAddr >> 12 & 0x03FF;
    uint32_t offset   = (uint32_t)virtualAddr       & 0x0FFF;

    pd_t* pd = paging_searchDirectory(pd_index);

    if(pd == NULL) {
        return NULL;
    }

    pt_t* pt = paging_searchTable(pd_index);

    if(pt == NULL) {
        return NULL;
    }

    return (uint32_t*)((pt[pt_index].phy_addr << 12) + offset);
}

uint32_t* paging_getVirtualBaseAddr(pd_t* pd)
{
    pd_t* pd_cmp = &pageDirectory;
    uint32_t pd_indexes = 0;

    for(int i = 0; i < 1024; i++)
    {
        if(pd == &pd_cmp[i]) {
            pd_indexes = i;
            break;
        }
    }

    return (uint32_t*) (pd_indexes << 22);
}
