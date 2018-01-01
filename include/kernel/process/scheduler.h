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

bool_t scheduler_add(task_t* task);
stack_layout_t* schedule_run(stack_layout_t* stack_layout);

#endif // __KERNEL_SCHEDULER_H__
