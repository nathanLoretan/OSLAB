/*
*   Paging
*/

#ifndef __KERNEL_PAGING_H__
#define __KERNEL_PAGING_H__

#include <lib/stdio.h>
#include <common/types.h>
#include <kernel/memory/memory_layout.h>

#define PDE_MAX  1024    // Max Page Directory Entries
#define PTE_MAX  1024    // Max Page Table Entries

#define PAGE_PRIV_USER        1
#define PAGE_PRIV_SUPERVISOR  0

#define PAGE_SIZE_4K  0
#define PAGE_SIZE_4M  1

struct pd {
    uint8_t  present        :1;     // Present
    uint8_t  read_write     :1;     // Read/Write (1) / Read-Only (0)
    uint8_t  privilege      :1;     // User (1) / Supervisor (0)
    uint8_t  write_through  :1;     // Write though
    uint8_t  cache_disable  :1;     // Cache disabled
    uint8_t  accessed       :1;     // accessed
    uint8_t  reserved       :1;     // Reserved
    uint8_t  size           :1;     // Page dize
    uint8_t  ignored        :1;     // Ignored
    uint8_t  available      :3;     // Available for OS
    uint32_t pt_addr        :20;    // Page table address (4-kb aligned)
}__attribute__((packed));

struct pt {
    uint8_t  present        :1;     // Present
    uint8_t  read_write     :1;     // Read/Write (1) / Read-Only (0)
    uint8_t  privilege      :1;     // User (1) / Supervisor (0)
    uint8_t  write_through  :1;     // Write Though
    uint8_t  cache_disable  :1;     // Cache disabled (1)
    uint8_t  accessed       :1;     // accessed
    uint8_t  dirty          :1;     // Dirty
    uint8_t  reserved       :1;     // Reserved
    uint8_t  global         :1;     // Global
    uint8_t  available      :3;     // Available for OS
    uint32_t phy_addr       :20;    // Physical page Address
}__attribute__((packed));

typedef struct pd pd_t;
typedef struct pt pt_t;

void pd_uInit(pd_t* pd);
void pt_uInit(pt_t* pt);

void pd_kInit(pd_t* pd);
void pt_kInit(pt_t* pt);

void paging_enable();
void paging_disable();

pd_t* paging_getPageDirectoryTable();

uint32_t paging_kGetDirectory(pd_t** pd); // Get a page directory for kernel
uint32_t paging_uGetDirectory(pd_t** pd); // Get a page directory for user

pd_t* paging_searchDirectory(uint32_t pd_index);
pt_t* paging_searchTable(uint32_t pd_index);

// pd_t* paging_getDirectory();

pt_t* paging_uGetTable(); // Allocate directly with process

// pd_t* paging_getKernelDirectory();
// pt_t* paging_getKernelTable();

uint32_t paging_alloc(pt_t* pt);
uint32_t paging_allocDriver(pt_t* pt, uint32_t phy);
void paging_free(pt_t* pt);

void paging_addTableToDirectory(pd_t* pd, pt_t* pt);

uint32_t* paging_getVirtualBaseAddr(pd_t* pd);
uint32_t* paging_getVirtualAddr(uint32_t pd_index, uint32_t pt_index, uint32_t offset);
uint32_t* paging_getPhysicalAddr(uint32_t* virtualAddr);

#endif // __KERNEL_PAGING_H__
