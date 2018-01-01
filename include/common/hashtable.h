/*
*   Hashtable
*/

#ifndef __COMMON_HASHTABLE_H__
#define __COMMON_HASHTABLE_H__

#include <common/types.h>
#include <common/list.h>
#include <kernel/memory/memory_manager.h>

typedef struct hashtable {
    size_t table_size;
    size_t data_size;
    list_t* table;
} hashtable_t;

int hashtable_init(hashtable_t* hashtable, size_t table_size, size_t data_size);
uint8_t* hashtable_search(hashtable_t* hashtable, size_t key);
int hashtable_inserte(hashtable_t* hashtable, size_t key, uint8_t* data);
int hashtable_delete(hashtable_t* hashtable, size_t key);

#endif // __COMMON_HASHTABLE_H__
