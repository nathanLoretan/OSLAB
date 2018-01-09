/*
*   Memory Manager
*/

#include <kernel/memory/memory_manager.h>

static memory_manager_t* ma;
// static memory_manager_t kernel_ma;

void mm_init(memory_manager_t* memory_manager, uint8_t* heap)
{
    // // If the size available for memory_manager is enough
    // if(size < sizeof(frame_t))
    // {
    //     kernel_ma.first = NULL;
    // }
    // else
    // {
        // kernel_ma.first = (frame_t*)heap;
        // kernel_ma.first->allocated = FALSE;
        // kernel_ma.first->prev = 0;
        // kernel_ma.first->next = 0;
        // kernel_ma.first->size = 0x3FFFFF; // Size available with the page table
        // // kernel_ma.first->size = size - sizeof(frame_t);
    // }

    memory_manager->first = (frame_t*)heap;
    memory_manager->first->allocated = FALSE;
    memory_manager->first->prev = 0;
    memory_manager->first->next = 0;
    memory_manager->first->size = 0x3FFFFF; // Size available with the page table

}
//
// void mm_process_init(memory_manager_t* memory_manager, uint8_t* heap, size_t size)
// {
//     // If the size available for memory_manager is enough
//     if(size < sizeof(frame_t))
//     {
//         memory_manager->first = NULL;
//     }
//     else
//     {
//         memory_manager->first = (frame_t*)heap;
//         memory_manager->first->allocated = FALSE;
//         memory_manager->first->prev = 0;
//         memory_manager->first->next = 0;
//         memory_manager->first->size = size - sizeof(frame_t);
//     }
// }

void mm_set(memory_manager_t* memory_manager)
{
    ma = memory_manager;
}

// void* process_malloc(size_t size)
// {
//     frame_t* new = NULL;
//
//     // Iterate to search a non-allocated frame
//     for(frame_t* frame = kernel_ma.first; frame != NULL; frame = frame->next)
//     {
//         if(frame->size > size + sizeof(frame_t) && !frame->allocated)
//         {
//             new = frame;
//             break;
//         }
//     }
//
//     if(new == NULL) {
//         return NULL;
//     }
//
//     // if the frame used is bigger than the memory needed
//     if(new->size >= size + sizeof(frame_t) + 1)
//     {
//         // Get the address of the memory exceeding
//         frame_t* tmp = (frame_t*)((size_t)new + sizeof(frame_t) + size);
//
//         tmp->size       = new->size - size - sizeof(frame_t);
//         tmp->prev       = new;
//         tmp->next       = new->next;
//         tmp->allocated  = FALSE;
//
//         if(tmp->next != NULL) {
//             tmp->next->prev = tmp;
//         }
//
//         new->size = size;
//         new->next = tmp;
//     }
//
//     new->allocated = TRUE;
//
//     return (void*)(((size_t)new) + sizeof(frame_t));
// }

void* malloc(size_t size)
{
    frame_t* new = NULL;

    // Iterate to search a non-allocated frame
    for(frame_t* frame = ma->first; frame != NULL; frame = frame->next)
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
