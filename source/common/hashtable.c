/*
*   Hashtable
*/

#include <common/hashtable.h>

static size_t hashtable_func(size_t key)
{
    // TODO:

    return 0;
}

int hashtable_init(hashtable_t* hashtable, size_t table_size, size_t data_size)
{
    hashtable->table = (list_t*) malloc(table_size);

    if(hashtable->table == NULL) {
        return -1;
    }

    hashtable->table_size = table_size;
    hashtable->data_size = data_size;

    for(size_t i = 0; i < table_size; i++) {
        list_init(&hashtable->table[i], data_size);
    }
}

uint8_t* hashtable_search(hashtable_t* hashtable, size_t key)
{
    size_t table_key = hashtable_func(key);

    return list_search(&hashtable->table[table_key], key);
}

int hashtable_inserte(hashtable_t* hashtable, size_t key, uint8_t* data)
{
    size_t table_key = hashtable_func(key);

    return list_add(&hashtable->table[table_key], key, data);
}

int hashtable_delete(hashtable_t* hashtable, size_t key)
{
    size_t table_key = hashtable_func(key);

    return list_delete(&hashtable->table[table_key], key);
}
