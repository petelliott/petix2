#include "list.h"
#include "../kdebug.h"
#include "../kmalloc.h"
#include <string.h>


void list_init(struct list *list, size_t item_size) {
    list->item_size = item_size;
    list->size = 0;
    list->cap  = 0;
    list->data = NULL;
}

void list_free(struct list *list) {
    kfree_sync(list_release(list));
}

void *list_release(struct list *list) {
    void *data = list->data;
    list->size = 0;
    list->cap  = 0;
    list->data = NULL;
    return data;
}

void *list_release_nt(struct list *list) {
    list_append(list, NULL);
    return list_release(list);
}

void list_append(struct list *list, void const *item) {
    kassert(list->size <= list->cap);
    if (list->size == list->cap) {
        list->cap  = (list->cap)? 2*list->cap : 32;
        list->data = krealloc_sync(list->data, list->cap * list->item_size);
        kassert(list->data);
    }
    list->size++;
    if (item == NULL) {
        memset(list_at(list, list->size - 1), 0, list->item_size);
    } else {
        memcpy(list_at(list, list->size - 1), item, list->item_size);
    }
}

void list_pop(struct list *list) {
    kassert(list->size > 0);
    list->size--;
}

void *list_at_unchecked(struct list *list, size_t index) {
    return list->data + (index * list->item_size);
}

void *list_at(struct list *list, size_t index) {
    kassert(index < list->size);
    return list_at_unchecked(list, index);
}
