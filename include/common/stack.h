/*
*   Stack (LIFO)
*/

#ifndef __COMMON_STACK_H__
#define __COMMON_STACK_H__

#include <common/types.h>
#include <kernel/memory/memory_manager.h>

typedef struct stack {
    size_t    top;
    size_t    bottom;
    size_t    STACK_SIZE;
    size_t    data_size;
    uint8_t*  data;
    uint8_t*  stack;
} stack_t;

int stack_init(stack_t* stack, size_t STACK_SIZE, size_t data_size);

uint8_t* stack_pop(stack_t* stack);
int stack_push(stack_t* stack, uint8_t* data);

bool_t stack_empty(stack_t* stack);
bool_t stack_full(stack_t* stack);

#endif // __COMMON_STACK_H__
