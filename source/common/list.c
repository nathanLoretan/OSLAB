/*
*   Linked List
*/

#include <common/list.h>

void list_init(list_t* list, size_t data_size)
{
    list->data_size = data_size;

    list->sentinel.next = list->sentinel.prev;
    list->sentinel.prev = list->sentinel.next;
    list->sentinel.data = NULL;
    list->sentinel.key  = (size_t)-1;

    list->iterator = NULL;
}

uint8_t* list_search(list_t* list, size_t key)
{
    list_obj_t* it_next = list->sentinel.next;
    list_obj_t* it_prev = list->sentinel.prev;

    while(it_next != it_prev && it_next->key != key && it_prev->key != key) {
        it_next = it_next->next;
        it_prev = it_prev->prev;
    }

    if(it_next->key == key) {
        return it_next->data;
    }
    else if(it_prev->key == key) {
        return it_prev->data;
    }

    return NULL;
}

int list_add(list_t* list, size_t key, uint8_t* data, bool_t unique_key)
{
    // Check if the key is alrady within the list
    if(list_search_obj(list, key) != NULL && unique_key == TRUE) {
        return -1;
    }

    list_obj_t* new_obj = (list_obj_t*) malloc(sizeof(list_obj_t));

    if(new_obj == NULL) {
        return -1;
    }

    new_obj->key  = key;
    new_obj->next = NULL;
    new_obj->prev = NULL;
    new_obj->data = (uint8_t*) malloc(list->data_size);

    if(new_obj->data == NULL) {
        return -1;
    }

    new_obj->next = list->sentinel.next;
    list->sentinel.next->prev = new_obj;
    list->sentinel.next = new_obj;
    new_obj->prev = &list->sentinel;

    return 0;
}

int list_insert(list_t* list, size_t key, uint8_t* data, bool_t unique_key, bool_t after_it)
{
    if(list->iterator == NULL) {
        return -1;
    }

    // Check if the key is alrady within the list
    if(list_search_obj(list, key) != NULL && unique_key == TRUE) {
        return -1;
    }

    list_obj_t* new_obj = (list_obj_t*) malloc(sizeof(list_obj_t));

    if(new_obj == NULL) {
        return -1;
    }

    new_obj->key  = key;
    new_obj->next = NULL;
    new_obj->prev = NULL;
    new_obj->data = (uint8_t*) malloc(list->data_size);

    if(new_obj->data == NULL) {
        return -1;
    }

    if(after_it)
    {
        new_obj->next = list->iterator->next;
        list->iterator->next->prev = new_obj;
        list->iterator->next = new_obj;
        new_obj->prev = list->iterator;
    }
    else
    {
        new_obj->prev = list->iterator->prev;
        list->iterator->prev->next = new_obj;
        list->iterator->prev = new_obj;
        new_obj->next = list->iterator;
    }

    return 0;
}

uint8_t* list_getFirst(list_t* list)
{
    return list->sentinel.next->data;
}

uint8_t* list_getLast(list_t* list)
{
    return list->sentinel.prev->data;
}

uint8_t* list_begin(list_t* list)
{
    list->iterator = list->sentinel.next;
    return list->iterator->data;
}

uint8_t* list_end(list_t* list)
{
    return list->sentinel.data;
}

uint8_t* list_next(list_t* list)
{
    list->iterator = list->iterator->next;
    return list->iterator->data;
}

int list_deleteFirst(list_t* list)
{
    list_obj_t* list_obj = list->sentinel.next;

    if(list_obj == NULL) {
        return -1;
    }

    list_obj->prev->next = list_obj->next;
    list_obj->next->prev = list_obj->prev;

    free(list_obj->data);
    free(list_obj);

    return 0;
}

int list_deleteLast(list_t* list)
{
    list_obj_t* list_obj = list->sentinel.prev;

    if(list_obj == NULL) {
        return -1;
    }

    list_obj->prev->next = list_obj->next;
    list_obj->next->prev = list_obj->prev;

    free(list_obj->data);
    free(list_obj);

    return 0;
}

int list_delete(list_t* list)
{
    if(list->iterator == NULL) {
        return -1;
    }

    list->iterator->prev->next = list->iterator->next;
    list->iterator->next->prev = list->iterator->prev;

    list_obj_t* new_iterator = list->iterator->next;

    free(list->iterator->data);
    free(list->iterator);

    list->iterator = new_iterator;

    return 0;
}

int list_deleteKey(list_t* list, size_t key)
{
    list_obj_t* list_obj;

    // Check if the key is within the list
    if((list_obj = list_search_obj(list, key)) == NULL) {
        return -1;
    }

    list_obj->prev->next = list_obj->next;
    list_obj->next->prev = list_obj->prev;

    free(list_obj->data);
    free(list_obj);

    return 0;
}

list_obj_t* list_search_obj(list_t* list, size_t key)
{
    list_obj_t* it_next = list->sentinel.next;
    list_obj_t* it_prev = list->sentinel.prev;

    while(it_next != it_prev && it_next->key != key && it_prev->key != key) {
        it_next = it_next->next;
        it_prev = it_prev->prev;
    }

    if(it_next->key == key) {
        return it_next;
    }
    else if(it_prev->key == key) {
        return it_prev;
    }

    return NULL;
}
