#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>

void *kmalloc(size_t size);
void *krealloc(void *ptr, size_t size);
void kfree(void *ptr);

#endif
