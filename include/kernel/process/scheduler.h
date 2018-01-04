/*
*   Scheduler
*/

#ifndef __KERNEL_SCHEDULER_H__
#define __KERNEL_SCHEDULER_H__

#include <common/types.h>
#include <kernel/gdt.h>
#include <kernel/process/task.h>
#include <kernel/memory/memory_manager.h>

#define SCHEDULER_NBR_TASKS  256
#define SCHEDULER_TIME_UNIT  1000   // us

void scheduler_init();
bool_t scheduler_add(task_t* task);
context_t* schedule_run(context_t* context);

#endif // __KERNEL_SCHEDULER_H__
