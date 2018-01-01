/*
*   Queue (FIFO)
*/

#ifndef __COMMON_QUEUE_H__
#define __COMMON_QUEUE_H__

#include <common/types.h>
#include <kernel/memory/memory_manager.h>

typedef struct queue {
    size_t    head;
    size_t    tail;
    size_t    queue_size;
    size_t    data_size;
    bool_t    isFull;
    bool_t    isEmpty;
    uint8_t*  data;
    uint8_t*  queue;
} queue_t;

int queue_init(queue_t* queue, size_t queue_size, size_t data_size);
bool_t queue_empty(queue_t* queue);
bool_t queue_full(queue_t* queue);
uint8_t* queue_dequeue(queue_t* queue);
int queue_enqueue(queue_t* queue, uint8_t* data);

#endif // __COMMON_QUEUE_H__
