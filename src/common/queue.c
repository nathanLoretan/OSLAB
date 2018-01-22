/*
*   Queue (FIFO)
*/

#include <common/queue.h>

int queue_init(queue_t* queue, size_t queue_size, size_t data_size)
{
    queue->queue = (uint8_t*) malloc(queue_size);
    queue->data  = (uint8_t*) malloc(data_size);

    if(queue->queue == NULL || queue->data == NULL) {
        return -1;
    }

    queue->head = 0;
    queue->tail = 0;

    queue->queue_size   = queue_size;
    queue->data_size = data_size;

    return 0;
}

bool_t queue_empty(queue_t* queue)
{
    return queue->isEmpty;
}

bool_t queue_full(queue_t* queue)
{
    return queue->isFull;
}

uint8_t* queue_dequeue(queue_t* queue)
{
    if(queue->isEmpty) {
        return NULL;
    }

    for(int i = 0; i < queue->data_size; i++) {
        queue->data[i] = queue->queue[(i + queue->head) % queue->queue_size];
    }

    queue->head = (queue->head + queue->data_size) % queue->queue_size;

    if(queue->tail == queue->head) {
        queue->isEmpty = TRUE;
    }

    queue->isFull = FALSE;

    return queue->data;
}

int queue_enqueue(queue_t* queue, uint8_t* data)
{
    if(queue->isFull == TRUE) {
        return -1;
    }

    for(int i = 0; i < queue->data_size; i++) {
        queue->queue[(i + queue->tail) % queue->queue_size] = data[i];
    }

    queue->tail = (queue->tail + queue->data_size) % queue->queue_size;

    if(queue->tail == queue->head) {
        queue->isFull = TRUE;
    }

    queue->isEmpty = FALSE;

    return 0;
}
