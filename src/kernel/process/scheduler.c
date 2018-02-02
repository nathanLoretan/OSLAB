/*
*   Scheduler
*/

#include <kernel/process/scheduler.h>

uint32_t task_cnt    = 0;
uint32_t task_id     = 0;
// uint32_t task_active = 0;

task_t  kernel_task;
task_t* active_task;
// task_t* tasks[SCHEDULER_task_cntS]; // first task correspond to the kernel process

list_t tasks_list;

void scheduler_init(memoryManager_t* memoryManager)
{
    // tasks[0] = &kernel_task;

    // Init the task list, data size == 0, the list will copy the pointer and
    // not the data
    list_init(&tasks_list, 0);

    // Give an id to the task, kernel task id == 0
    kernel_task.id = task_id;
    kernel_task.memoryManager = *memoryManager;

    list_add(&tasks_list, task_id, (uint8_t*)&kernel_task, TRUE);

    task_id = (task_id + 1) % SCHEDULER_NBR_TASKS;
    task_cnt++;

    active_task = (task_t*)list_begin(&tasks_list);
}

bool_t scheduler_delete(task_t* task)
{
    if(list_deleteKey(&tasks_list, task->id) == 0) {
        task_cnt--;
        return TRUE;
    }

    return FALSE;
}

bool_t scheduler_add(task_t* task)
{
    // TODO: LOCK

    // if(task_cnt >= SCHEDULER_task_cntS) {
    //     return FALSE;
    // }
    //
    // // The first task correpond to the main process of the kernel
    // task_cnt++;
    // tasks[task_cnt] = task;
    //
    // return TRUE;

    uint8_t loop = task_id;

    // Add the task to the list if there is enough space
    while(list_add(&tasks_list, task_id, (uint8_t*)task, TRUE) == -1) {
        task_id = (task_id + 1) % SCHEDULER_NBR_TASKS;

        // Check if no place are find
        if(loop == task_id) {
            return FALSE;
        }
    }

    // Give an id to the task
    task->id = task_id;

    task_id = (task_id + 1) % SCHEDULER_NBR_TASKS;
    task_cnt++;

    return TRUE;

    // TODO: UNLOCK
}

context_t* schedule_switchContext(context_t* context)
{
    // TODO: LOCK

    // // no task, no need to schedule
    // if(task_cnt == 0) {
    //     return context;
    // }
    //
    // // If the context of the task is not defined TODO: remove the task
    // if(tasks[(task_active + 1) % (task_cnt + 1)]->context == NULL) {
    //     printf("TASK %x is NULL\n", (task_active + 1) % (task_cnt + 1));
    //     return context;
    // }
    //
    // // Save the registers of the current task
    // tasks[task_active]->context = context;
    //
    // // Select the new task to schedule
    // task_active = (task_active + 1) % (task_cnt + 1);
    //
    // // Select the memory space of the process
    // mm_set(&tasks[task_active]->memoryManager);
    //
    // return tasks[task_active]->context;
    //
    task_t* next_task;

    // no task, no need to schedule
    if(task_cnt == 0) {
        return context;
    }

    // Get the next task to execute. If the next element is NULL it is because
    // it is the sentinel of the list
    if((next_task = (task_t*)list_next(&tasks_list)) == NULL)
    {
        // If two times NULL, there is certainly a problem
        if((next_task = (task_t*)list_next(&tasks_list)) == NULL) {
            return context;
        }
    }

    // Save the context of the task
    active_task->context = context;

    active_task = next_task;

    // Select the memory space of the process
    mm_set(&active_task->memoryManager);

    return active_task->context;

    // TODO: UNLOCK
}

void scheduler_yield()
{
    asm volatile("int %0": :"i" (IRQ_YIELD));
}

task_t* scheduler_getCurrentTask()
{
    return active_task;//tasks[task_active];
}
