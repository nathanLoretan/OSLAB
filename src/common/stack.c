/*
*   Stack (LIFO)
*/

#include <common/stack.h>

int stack_init(stack_t* stack, size_t STACK_SIZE, size_t data_size)
{
    stack->stack = (uint8_t*) malloc(STACK_SIZE);
    stack->data  = (uint8_t*) malloc(data_size);

    if(stack->stack == NULL || stack->data == NULL) {
        return -1;
    }

    stack->top    = 0;
    stack->bottom = 0;

    stack->STACK_SIZE = STACK_SIZE;
    stack->data_size  = data_size;

    return 0;
}

bool_t stack_empty(stack_t* stack)
{
    if(stack->bottom == stack->top) {
        return TRUE;
    }

    return FALSE;
}

bool_t stack_full(stack_t* stack)
{
    if(stack->top >= stack->STACK_SIZE - 1) {
        return TRUE;
    }

    return FALSE;
}

uint8_t* stack_pop(stack_t* stack)
{
    if(stack_empty(stack)) {
        return NULL;
    }

    stack->top = stack->top - stack->data_size;

    for(uint8_t i = 0; i < stack->data_size; i++) {
        stack->data[i] = stack->stack[i + stack->top];
    }

    return stack->data;
}

int stack_push(stack_t* stack, uint8_t* data)
{
    if((stack->top + stack->data_size) >= stack->STACK_SIZE) {
        return -1;
    }

    for(uint8_t i = 0; i < stack->data_size; i++) {
        stack->stack[i + stack->top] = data[i];
    }

    stack->top = stack->top + stack->data_size;

    return 0;
}
