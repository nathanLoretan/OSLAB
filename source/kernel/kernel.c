#include <common/types.h>
#include <kernel/gdt.h>
#include <kernel/pci.h>
#include <kernel/memory/paging.h>
#include <kernel/process/task.h>
#include <kernel/process/scheduler.h>
#include <kernel/interrupt/interrupt_manager.h>
#include <kernel/memory/memory_manager.h>
#include <drivers/pit.h>
#include <drivers/keyboard.h>
#include <syscalls/print.h>

#include <test/test.h>

pd_t* kernel_pd;
pt_t* kernel_pt;

// TODO:    - determine why pt_init() fail if page not present

void kmain(const void* kernel_space)
{
    printf("OSLAB Project\n");

    gdt_init();
    printf("GDT initialized\n");

    im_init();
    printf("IDT initialized\n");

    // pci_init();
    // printf("PCI initialized\n");

    // kb_init();
    // printf("Keyboard initialized\n");

    uint32_t* memupper = (uint32_t*)(((size_t)kernel_space) + 8);
    size_t  heap = 10*1024*1024;
    mm_init((uint8_t*)heap, (*memupper)*1024 - (size_t)heap - 10*1024);

    kernel_pt = paging_getKernelTable();
    kernel_pd = paging_getKernelDirectory();
    paging_addTableToDirectory(kernel_pd, kernel_pt);
    paging_enable();

    scheduler_init();
    printf("Scheduler initialized\n");

    // printfHex32((uint32_t)kernel_pd);printf("\n");
    // printfHex32((uint32_t)kernel_pt);printf("\n");

    pit_set_timer(SCHEDULER_TIME_UNIT);

    im_enable_interrupt();
    printf("Interrupts activated\n");

    // Test Function
    testPaging();

    while(1)
    {
        if(!im_queue_isEmpty()) {

            isr_t handler = im_queue_get();
            if(handler != NULL) {
                handler();
            }
        }
    }
}
