/*
*   Timer
*/

#ifndef __COMMON_TIMER_H__
#define __COMMON_TIMER_H__

#include <common/types.h>
#include <common/event.h>
#include <common/list.h>
#include <kernel/process/scheduler.h>

typedef void (*timer_handler_t)();

typedef struct timer {
    size_t id;
    bool_t periodic;
    uint32_t interval;
    uint32_t nbr_ticks;
    uint32_t remaining_ticks;
    eventqueue_t* queue;
    event_t* event;
} timer_t;

void timer_init(timer_t* timer, eventqueue_t* queue, event_t event,
                size_t interval, bool_t periodic, size_t id);
void timer_start(timer_t* timer);
void timer_stop(timer_t* timer);
void tick();

#endif // __COMMON_TIMER_H__
