/*
*   Scheduler
*/

#include <kernel/process/scheduler.h>
#include <syscalls/print.h>

uint8_t nbr_task = 0;
uint8_t current_task = 0;
task_t kernel_task;
task_t* tasks[SCHEDULER_NBR_TASKS];

void scheduler_init()
{
    task_t kernel_task;
    task_init(&kernel_task, NULL, 4096);
    tasks[0] = &kernel_task;
}

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

context_t* schedule_run(context_t* context)
{
    // no task, no need to schedule
    if(nbr_task == 0) {
        return context;
    }

    // If the context of the task is not defined TODO: remove the task
    if(tasks[(current_task + 1) % (nbr_task + 1)]->context == NULL) {
        return context;
    }

    // Save the registers of the current task
    tasks[current_task]->context = context;

    // Select the new task to schedule
    current_task = (current_task + 1) % (nbr_task + 1);

    // Select the memory space of the process
    mm_set(&tasks[current_task]->memory_manager);

    return tasks[current_task]->context;
}
