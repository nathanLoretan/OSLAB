/*
*   Task
*/

#include <kernel/process/task.h>

static uint32_t pointer = 0;

void task_init(task_t* task, run_t run, size_t size)
{
    task->heap  = (uint8_t*)pt_getAddr(&kernel_pt[10 + pointer++]);

    // mm_process_init(&task->memory_manager, task->heap, size);

    // Define where to save the cpu registers
    task->context = (context_t*)(task->heap + 0x1000 - sizeof(context_t));

    task->context->cr3 = (uint32_t) kernel_pd;

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
