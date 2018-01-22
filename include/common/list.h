/*
*   Linked List
*/

#ifndef __COMMON_list_H__
#define __COMMON_list_H__

#include <common/types.h>
#include <lib/string.h>
#include <kernel/memory/memory_manager.h>

typedef struct list_obj {
    size_t   key;
    uint8_t* data;
    struct list_obj* next;
    struct list_obj* prev;
} list_obj_t;

typedef struct list {
    size_t data_size;
    list_obj_t sentinel;
    list_obj_t* iterator;
} list_t;

void list_init(list_t* list, size_t data_size);

int list_add(list_t* list, size_t key, uint8_t* data, bool_t unique_key);
int list_insert(list_t* list, size_t key, uint8_t* data, bool_t unique_key, bool_t after_it);

uint8_t* list_search(list_t* list, size_t key);
uint8_t* list_getFirst(list_t* list);
uint8_t* list_getLast(list_t* list);

int list_delete(list_t* list);
int list_deleteKey(list_t* list, size_t key);
int list_deleteFirst(list_t* list);
int list_deleteLast(list_t* list);

uint8_t* list_begin(list_t* list);
uint8_t* list_end(list_t* list);
uint8_t* list_next(list_t* list);

list_obj_t* list_search_obj(list_t* list, size_t key);

#endif // __COMMON_list_H__
