/*
*   Event
*/

#include <common/event.h>

int event_init(event_t* event, event_handler_t handler, size_t data_size, size_t id)
{
    event->handler  = handler;
    event->id       = id;
    event->data     = (uint8_t*) malloc(data_size);
}

int eventqueue_init(eventqueue_t* queue, size_t nbr_event, size_t data_size)
{
    size_t queue_size = nbr_event * sizeof(event_t);

    return queue_init(queue, queue_size, data_size);
}

int eventqueue_push(eventqueue_t* queue, event_t* event)
{
    return queue_enqueue(queue, (uint8_t*)event);
}

event_t* eventqueue_pop(eventqueue_t* queue)
{
    return (event_t*)queue_dequeue(queue);
}
