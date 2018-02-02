/*
*   Memory Manager
*/

#ifndef __KERNEL_MEMORY_MANAGER_H__
#define __KERNEL_MEMORY_MANAGER_H__

#include <common/types.h>

struct frame {
    struct frame *next;
    struct frame *prev;
    bool_t allocated;
    size_t size;
} __attribute__((packed));

struct memoryManager {
    size_t size;
    struct frame* first;
} __attribute__((packed));

typedef struct frame          frame_t;
typedef struct memoryManager memoryManager_t;

void  mm_init(memoryManager_t* memoryManager, uint8_t* heap);
void  mm_set(memoryManager_t* memoryManager);

void* malloc(size_t size);
void  free(void* ptr);

#endif // __KERNEL_MEMORY_MANAGER_H__
