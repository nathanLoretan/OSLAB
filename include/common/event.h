/*
*   Event
*/

#ifndef __COMMON_EVENT_H__
#define __COMMON_EVENT_H__

#include <common/types.h>
#include <common/queue.h>

typedef void (*event_handler_t)();

typedef struct event {
    size_t id;
    event_handler_t handler;
    uint8_t* data;
} event_t;

typedef queue_t eventqueue_t;

int event_init(event_t* event, event_handler_t handler, size_t data_size, size_t id);
int eventqueue_init(eventqueue_t* queue, size_t nbr_event, size_t data_size);
int eventqueue_push(eventqueue_t* queue, event_t* event);
event_t eventqueue_pop(eventqueue_t* queue);

#endif // __COMMON_EVENT_H__
