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

void taskD()
{
    char* text = (char*) malloc(6);

    // text[0] = 'H';
    // text[1] = 'E';
    // text[2] = 'L';
    // text[3] = 'L';
    // text[4] = 'O';
    // text[5] = '\0';
    //
    // printf(text);

    printf("\ntext ");
    printfHex32((uint32_t)text);
    free(text);
    text = NULL;
    text = (char*) malloc(6);
    printf("\ntext ");
    printfHex32((uint32_t)text);

    char* text2 = (char*) malloc(6);
    printf("\ntext2 ");
    printfHex32((uint32_t)text2);
    free(text);
    text = NULL;
    text = (char*) malloc(5);
    printf("\ntext ");
    printfHex32((uint32_t)text);

    char* text3 = (char*) malloc(6);
    printf("\ntext3 ");
    printfHex32((uint32_t)text3);
    free(text2);
    text2 = NULL;
    text2 = (char*) malloc(5);
    printf("\ntext2 ");
    printfHex32((uint32_t)text2);

    while(1) {
    }

    printf("End taskA\n");
}

void taskE()
{
    char* text = (char*) malloc(6);

    // text[0] = 'W';
    // text[1] = 'O';
    // text[2] = 'R';
    // text[3] = 'L';
    // text[4] = 'D';
    // text[5] = '\0';
    //
    // printf(text);

    printf("\n");
    printfHex32((uint32_t)text);
    free(text);
    text = NULL;
    text = (char*) malloc(6);
    printf("\n");
    printfHex32((uint32_t)text);

    char* text2 = (char*) malloc(6);
    printf("\n");
    printfHex32((uint32_t)text2);
    free(text);
    text = NULL;
    text = (char*) malloc(6);
    printf("\n");
    printfHex32((uint32_t)text);

    while(1) {
    }
}

void taskF()
{
    char* text = (char*) malloc(6);

    text[0] = '!';
    text[1] = '!';
    text[2] = '!';
    text[3] = '!';
    text[4] = '!';
    text[5] = '\0';

    printf(text);

    printf("\n");
    printfHex32((uint32_t)text);
    free(text);
    text = NULL;
    text = (char*) malloc(6);
    printf("\n");
    printfHex32((uint32_t)text);

    char* text2 = (char*) malloc(6);
    printf("\n");
    printfHex32((uint32_t)text2);
    free(text);
    text = NULL;
    text = (char*) malloc(6);
    printf("\n");
    printfHex32((uint32_t)text);

    while(1) {
    }

    printf("End taskC\n");
}

void allocationTest()
{
    task_t task1;
    task_t task2;
    task_t task3;

    task_init(&task1, taskD, 4096);
    task_init(&task2, taskE, 4096);
    task_init(&task3, taskF, 4096);

    scheduler_add(&task1);
    scheduler_add(&task2);
    scheduler_add(&task3);
}

void taskA()
{
    while(1) {
        printf("A");
    }

    printf("End taskA\n");
}

void taskB()
{
    while(1) {
        printf("B");
    }

    printf("End taskB\n");
}

void taskC()
{
    while(1) {
        printf("C");
    }

    printf("End taskC\n");
}

task_t task1;
task_t task2;
task_t task3;

void schedulerTest()
{
    task_init(&task1, taskA, 4096);
    task_init(&task2, taskB, 4096);
    task_init(&task3, taskC, 4096);

    scheduler_add(&task1);
    scheduler_add(&task2);
    scheduler_add(&task3);
}

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

    scheduler_init();
    printf("Scheduler initialized\n");

    uint32_t* memupper = (uint32_t*)(((size_t)kernel_space) + 8);
    size_t  heap = 10*1024*1024;
    mm_init((uint8_t*)heap, (*memupper)*1024 - (size_t)heap - 10*1024);

    pd_init(kernel_pd);
    pt_init(kernel_pt);
    pd_add(kernel_pd, kernel_pt);
    pd_select(kernel_pd);
    paging_enable();

    pit_set_timer(SCHEDULER_TIME_UNIT);

    im_enable_interrupt();
    printf("Interrupts activated\n");

    schedulerTest();

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
