/*
*   Task
*/

#include <kernel/process/task.h>

void task_init(task_t* task, run_t run)
{
    task->pd = paging_getDirectory();
    task->pt = paging_getTable();

    paging_addTableToDirectory(task->pd, task->pt);

    // /!\ the paging must be enabled
    // heap and stack addressed to virtual memory regarding the Directory and the table
    task->heap  = (uint8_t*)paging_getVirtualBaseAddr(task->pd);
    task->stack = (uint8_t*)task->heap + (1024 * 0x1000);
    task->context = (context_t*)(task->stack - sizeof(context_t));

    // Configure memory manager for dynamic allocation
    mm_init(&task->memoryManager, task->heap);

    // cr3 points to the page directory table
    task->context->cr3 = (uint32_t) paging_getPageDirectories();

    // General Purpose Registers
    task->context->eax = 0;
    task->context->ebx = 0;
    task->context->ecx = 0;
    task->context->edx = 0;

    seg_select_t code_seg = gdt_code_seg_select();

    // Segment Registers
    task->context->cs = *(uint32_t*)&code_seg;
    // task->context->ds = ;
    // task->context->es = ;
    // task->context->ss = ;
    // task->context->fs = ;
    // task->context->gs = ;

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
