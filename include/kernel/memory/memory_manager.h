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

struct memory_manager {
    size_t         size;
    struct frame*  first;
} __attribute__((packed));

typedef struct frame          frame_t;
typedef struct memory_manager memory_manager_t;

void  mm_init(uint8_t* heap, size_t size);
void  mm_process_init(memory_manager_t* memory_manager, uint8_t* heap, size_t size);
void  mm_set(memory_manager_t* memory_manager);

void* process_malloc(size_t size);
void* malloc(size_t size);
void  free(void* ptr);

#endif // __KERNEL_MEMORY_MANAGER_H__
