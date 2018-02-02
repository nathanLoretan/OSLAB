#include <common/types.h>
#include <lib/stdio.h>
#include <lib/string.h>

#include <kernel/gdt.h>
#include <kernel/memory/paging.h>
#include <kernel/process/task.h>
#include <kernel/process/scheduler.h>
#include <kernel/interrupt/interrupt_manager.h>
#include <kernel/memory/memory_manager.h>
#include <kernel/memory/memory_layout.h>

#include <drivers/pit.h>
#include <drivers/ata.h>
#include <drivers/pci.h>
#include <drivers/sata.h>
#include <drivers/drivers.h>
#include <drivers/keyboard.h>

#include <test/test.h>

pd_t* kernel_pd;
pt_t* kernel_pt;
memoryManager_t kernel_memoryManager;

void kmain(const void* kernel_space)
{
    // printf("  ____   _____ _               ____  \n");
    // printf(" / __ \\ / ____| |        /\\   |  _ \\ \n");
    // printf("| |  | | (___ | |       /  \\  | |_) |\n");
    // printf("| |  | |\\___ \\| |      / /\\ \\ |  _ < \n");
    // printf("| |__| |____) | |____ / ____ \\| |_) |\n");
    // printf(" \\____/|_____/|______/_/    \\_\\____/ \n");
    // printf("=====================================\n");

    gdt_init();
    printf("GDT initialized\n");

    im_init();
    printf("Interrupt initialized\n");

    // Initialize the memory manager with heap at virtual address 1MB
    mm_init(&kernel_memoryManager, (uint8_t*)KERNEL_HEAP);
    mm_set (&kernel_memoryManager);

    // drivers_init();
    // printf("Drivers list initialized\n");
    //
    // pci_init();
    // printf("PCI initialized\n");

    pit_set_timer(SCHEDULER_TIME_UNIT);
    printf("Programmable interrupt timer set to %32x\n", SCHEDULER_TIME_UNIT);

    scheduler_init(&kernel_memoryManager);
    printf("Scheduler initialized\n");

    im_enable_interrupt();
    printf("Interrupts activated\n");

    /* Test Function */
    testScheduler();
    // testAta();
    // testSata();

    // testAlloc();
    // testPaging();

    // kb_init();
    // printf("Keyboard initialized\n");

    while(1)
    {
        // if(!im_queue_isEmpty()) {
        //
        //     isr_t handler = im_queue_get();
        //     if(handler != NULL) {
        //         handler();
        //     }
        // }
    }
}
