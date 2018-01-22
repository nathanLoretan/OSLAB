/*
*   Scheduler
*/

#include <kernel/process/scheduler.h>

uint8_t nbr_task = 0;
uint8_t current_task = 0;
task_t kernel_task;
task_t* tasks[SCHEDULER_NBR_TASKS]; // first task correspond to the kernel process

void scheduler_init()
{
    extern pd_t* kernel_pd;
    extern pt_t* kernel_pt;

    kernel_task.pd = kernel_pd;
    kernel_task.pt = kernel_pt;

    tasks[0] = &kernel_task;
}

bool_t scheduler_add(task_t* task)
{
    // TODO: LOCK

    if(nbr_task >= SCHEDULER_NBR_TASKS) {
        return FALSE;
    }

    // The first task correpond to the main process of the kernel
    nbr_task++;
    tasks[nbr_task] = task;

    return TRUE;

    // TODO: UNLOCK
}

context_t* schedule_switchContext(context_t* context)
{
    // TODO: LOCK

    // no task, no need to schedule
    if(nbr_task == 0) {
        return context;
    }

    // If the context of the task is not defined TODO: remove the task
    if(tasks[(current_task + 1) % (nbr_task + 1)]->context == NULL) {
        printf("TASK %x is NULL\n", (current_task + 1) % (nbr_task + 1));
        return context;
    }

    // Save the registers of the current task
    tasks[current_task]->context = context;

    // Select the new task to schedule
    current_task = (current_task + 1) % (nbr_task + 1);

    // Select the memory space of the process
    mm_set(&tasks[current_task]->memoryManager);

    return tasks[current_task]->context;

    // TODO: UNLOCK
}

task_t* scheduler_getCurrentTask()
{
    return tasks[current_task];
}
