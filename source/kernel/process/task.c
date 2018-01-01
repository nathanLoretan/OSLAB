/*
*   Task
*/

#include <kernel/process/task.h>

void task_init(task_t* task, run_t run, size_t size)
{
    task->heap  = process_malloc(size);
    task->stack = task->heap + size;

    mm_process_init(&task->memory_manager, task->heap, size);

    // Define where to save the cpu registers
    task->stack_layout = (stack_layout_t*)(task->stack - sizeof(stack_layout_t));

    // // General Purpose Registers
    task->stack_layout->eax = 0;
    task->stack_layout->ebx = 0;
    task->stack_layout->ecx = 0;
    task->stack_layout->edx = 0;

    // Segment Registers
    task->stack_layout->cs = gdt_code_seg_select();
    // task->stack_layout->ds = ;
    // task->stack_layout->es = ;
    // task->stack_layout->ss = ;
    // task->stack_layout->fs = ;
    // task->stack_layout->gs = ;

    // Index Registers
    task->stack_layout->esi = 0;
    task->stack_layout->edi = 0;

    // Pointer Registers
    task->stack_layout->ebp = 0;
    // task->stack_layout->esp = ;
    task->stack_layout->eip = (uint32_t)run;

    // EFLAGS Register
    task->stack_layout->eflags = 0x202;
}
