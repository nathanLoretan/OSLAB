/*
*   Scheduler
*/

#include <kernel/process/scheduler.h>
#include <syscalls/print.h>

uint8_t nbr_task = 0;
uint8_t current_task = NULL;
task_t* tasks[SCHEDULER_NBR_TASKS];

bool_t scheduler_add(task_t* task)
{
    if(nbr_task >= SCHEDULER_NBR_TASKS) {
        return FALSE;
    }

    // The first task correpond to the main process of the kernel
    nbr_task++;
    tasks[nbr_task] = task;

    return TRUE;
}

stack_layout_t* schedule_run(stack_layout_t* stack_layout)
{
    // no task, no need to schedule
    if(nbr_task == 0) {
        return stack_layout;
    }

    // Save the registers of the current task
    tasks[current_task]->stack_layout = stack_layout;

    // Select the new task to schedule
    current_task = (current_task + 1) % (nbr_task + 1);

    printfHex32(current_task);
    printf("\n");

    // Select the memory space of the process
    mm_set(&tasks[current_task]->memory_manager);

    return tasks[current_task]->stack_layout;
}
