#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>

void *kmalloc(size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

void *kmalloc_sync(size_t size);
void *krealloc_sync(void *ptr, size_t size);
void kfree_sync(void *ptr);

char *kstrdup_sync(char const *str);

#endif
