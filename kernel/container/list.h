#ifndef CONTAINER_LIST_H
#define CONTAINER_LIST_H

#include <stddef.h>
#include "../kdebug.h"

struct list {
    size_t item_size;
    size_t size;
    size_t cap;
    void *data;
};

void list_init(struct list *list, size_t item_size);
void list_free(struct list *list);
void *list_release(struct list *list);
void *list_release_nt(struct list *list);

void list_append(struct list *list, void const *item);
#define list_appendv(list, item) {                                      \
        kassert(sizeof(item) == (list)->item_size);                     \
        typeof(item) _value = item;                                     \
        list_append(list, &_value);                                     \
    }

void list_pop(struct list *list);

void *list_at_unchecked(struct list *list, size_t index);
void *list_at(struct list *list, size_t index);

#define list_foreach(type, name, list)                                  \
    size_t _i = 0;                                                      \
    for (type name = list_at(&list, _i); _i < list.size; ++_i, name=list_at_unchecked(&list,_i))


#endif
