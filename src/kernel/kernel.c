#include <common/types.h>
#include <lib/stdio.h>
#include <lib/string.h>

#include <kernel/gdt.h>
#include <kernel/memory/paging.h>
#include <kernel/process/task.h>
#include <kernel/process/scheduler.h>
#include <kernel/interrupt/interrupt_manager.h>
#include <kernel/memory/memory_manager.h>

#include <drivers/pit.h>
#include <drivers/ata.h>
#include <drivers/pci.h>
#include <drivers/sata.h>
#include <drivers/drivers.h>
#include <drivers/keyboard.h>

#include <test/test.h>

void testSata()
{
    printf("init: %8x\n", sata_init());

    char textRead[512];
    char textWrite[512] = "Hello World\n";

    printf("write: %8x\n", sata_write28(0, 1, 1, (uint16_t*)textWrite));

    strncpy(textWrite, "Bonjour le monde\n", sizeof("Bonjour le monde\n"));

    printf("read: %8x\n", sata_read28(0, 1, 1, (uint16_t*)textRead));
    printf(textRead);
    printf(textWrite);
    while(1){}
}

void testAta()
{
    char textWrite[13] = "Hello World\n";
    char textRead[13];

    printf("ata pm: %8x\n", ata_identify(ATA_PRIMARY_MASTER));
    printf("write: %8x\n", ata_write28(ATA_PRIMARY_MASTER, 0, (uint8_t*)textWrite, 13));
    printf("flush: %8x\n", ata_flush(ATA_PRIMARY_MASTER));
    printf("read: %8x\n", ata_read28(ATA_PRIMARY_MASTER, 0, (uint8_t*)textRead, 13));
    printf(textWrite);
    printf(textRead);

    // printf("ata pm: %8x\n", ata_identify(ATA_SECONDARY_SLAVE));
    // printf("write: %8x\n", ata_write28(ATA_SECONDARY_SLAVE, 0, (uint8_t*)textWrite, 13));
    // printf("flush: %8x\n", ata_flush(ATA_SECONDARY_SLAVE));
    // printf("read: %8x\n", ata_read28(ATA_SECONDARY_SLAVE, 0, (uint8_t*)textRead, 13));
    // printf(textWrite);
    // printf(textRead);
}

pd_t* kernel_pd;
pt_t* kernel_pt;
memoryManager_t kernel_memoryManager;

void kmain(const void* kernel_space)
{
    printf("OSLAB Project\n");

    gdt_init();
    printf("GDT initialized\n");

    im_init();
    printf("IDT initialized\n");

    // kb_init();
    // printf("Keyboard initialized\n");

    kernel_pt = paging_getKernelTable();
    kernel_pd = paging_getKernelDirectory();
    paging_addTableToDirectory(kernel_pd, kernel_pt);
    paging_enable();

    // Initialize the memory manager with heap at virtual address 0x00100000
    mm_init(&kernel_memoryManager, (uint8_t*)0x00001000);
    mm_set(&kernel_memoryManager);

    drivers_init();
    printf("Drivers list initialized\n");

    pci_init();
    printf("PCI initialized\n");

    testSata();
    // printf("%8x\n", sata_init());

    pit_set_timer(SCHEDULER_TIME_UNIT);

    im_enable_interrupt();
    printf("Interrupts activated\n");

    scheduler_init();
    printf("Scheduler initialized\n");

    /* Test Function */
    // testPaging();
    // testScheduler();
    // testAlloc();
    // testAta();

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
