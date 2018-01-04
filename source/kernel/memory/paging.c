/*
*   Paging
*/

#include <kernel/memory/paging.h>

void paging_enable()
{
    asm volatile ("movl %cr0, %eax; orl $0x80000000, %eax; movl %eax, %cr0;");
}

void paging_disable()
{

}

void pd_init(pd_t* pd)
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

void pt_init(pt_t* pt)
{
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
}

void pd_add(pd_t* pd, pt_t* pt)
{
    pd->present     = 1;
    pd->read_write  = 1;
    pd->pt_addr     = (uint32_t)pt >> 12;
}

void pd_select(pd_t* pd)
{
    asm volatile ("movl %%eax, %%cr3" :: "a" (pd)); // load PDPT into CR3
}

uint32_t pt_getAddr(pt_t* pt)
{
    return pt->phy_addr << 12;
}
