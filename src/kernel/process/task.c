/*
*   Task
*/

#include <kernel/process/task.h>

void task_start()
{
    // Manage the lifetime of a task here
    // 1. get task from scheduler
    // 2. run the function for the task
    // 3. when finish remove all the different element needed
}

void task_init(task_t* task, run_t run)
{
    task->pd = paging_uGetDirectory();
    // task->pt = paging_uGetTable();
    pd_uInit(task->pd);
    pt_uInit(task->pt);

    paging_addTableToDirectory(task->pd, task->pt);

    // heap and stack addressed to virtual memory regarding the Directory and the table
    task->heap    = (uint8_t*)paging_getVirtualBaseAddr(task->pd);
    task->stack   = (uint8_t*)task->heap + 0x3FFFFF;
    task->context = (context_t*)(task->stack - sizeof(context_t));

    // Configure memory manager for dynamic allocation
    mm_init(&task->memoryManager, task->heap);

    // cr3 points to the page directory table
    task->context->cr3 = (uint32_t) paging_getPageDirectoryTable();

    // General Purpose Registers
    task->context->eax = 0;
    task->context->ebx = 0;
    task->context->ecx = 0;
    task->context->edx = 0;

    seg_select_t code_seg = gdt_kCode_segSelect();
    seg_select_t data_seg = gdt_kData_segSelect();

    // Segment Registers
    task->context->cs = *(uint32_t*)&code_seg;
    task->context->ds = *(uint32_t*)&data_seg;
    // task->context->es = ds;
    // task->context->ss = ds;
    // task->context->fs = ds;
    // task->context->gs = ds;

    // Index Registers
    task->context->esi = 0;
    task->context->edi = 0;

    // Pointer Registers
    task->context->ebp = 0;
    task->context->esp = (uint32_t)task->context;
    task->context->eip = (uint32_t)run;

    // EFLAGS Register
    task->context->eflags = 0x202;
}
