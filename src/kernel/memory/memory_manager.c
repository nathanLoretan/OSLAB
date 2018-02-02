/*
*   Memory Manager
*/

#include <kernel/memory/memory_manager.h>

#include <lib/stdio.h>

static memoryManager_t* current_ma;

void mm_init(memoryManager_t* memoryManager, uint8_t* heap)
{
    memoryManager->first = (frame_t*)heap;
    memoryManager->first->allocated = FALSE;
    memoryManager->first->prev = 0;
    memoryManager->first->next = 0;
    memoryManager->first->size = 0x003FFFFF; // Size available with the page table
}

void mm_set(memoryManager_t* memoryManager)
{
    current_ma = memoryManager;
}

void* malloc(size_t size)
{
    frame_t* new = NULL;

    // Iterate to search a non-allocated frame
    for(frame_t* frame = current_ma->first; frame != NULL; frame = frame->next)
    {
        if(frame->size > size + sizeof(frame_t) && !frame->allocated)
        {
            new = frame;
            break;
        }
    }

    if(new == NULL) {
        return NULL;
    }

    // if the frame used is bigger than the memory needed
    if(new->size >= size + sizeof(frame_t) + 1)
    {
        // Get the address of the memory exceeding
        frame_t* tmp = (frame_t*)((size_t)new + sizeof(frame_t) + size);

        tmp->size       = new->size - size - sizeof(frame_t);
        tmp->prev       = new;
        tmp->next       = new->next;
        tmp->allocated  = FALSE;

        if(tmp->next != NULL) {
            tmp->next->prev = tmp;
        }

        new->size = size;
        new->next = tmp;
    }

    new->allocated = TRUE;

    return (void*)(((size_t)new) + sizeof(frame_t));
}

void free(void* ptr)
{
    // Get the frame corresponding to the memory allocated
    frame_t* frame = (frame_t*)((size_t)ptr - sizeof(frame_t));

    frame->allocated = FALSE;

    // Merge the free memory space with the previous one
    if(frame->prev != NULL && !frame->prev->allocated)
    {
        frame->prev->next  = frame->next;
        frame->prev->size += frame->size + sizeof(frame_t);

        if(frame->next != NULL) {
            frame->next->prev = frame->prev;
        }

        frame = frame->prev;
    }

    // Merge the free memory space with the next one
    if(frame->next != NULL && !frame->next->allocated)
    {
        frame->size += frame->next->size + sizeof(frame_t);
        frame->next  = frame->next->next;

        if(frame->next != NULL) {
            frame->next->prev = frame;
        }
    }
}
