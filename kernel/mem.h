#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>
#include "sync.h"

#define PAGE_SIZE 4096

typedef uint32_t page_t;

// currently only continuous ram can be used
void mem_init(uintptr_t base, uintptr_t initbrk, uintptr_t length);

// don't call these directly
int kbrk(void *addr);
void *ksbrk(intptr_t increment);

page_t alloc_page(void);
void free_page(page_t page);


void *alloc_page_ptr(void);
void free_page_ptr(void *page);

extern petix_lock_t memlock;

page_t alloc_page_sync(void);
void free_page_sync(page_t page);

void *alloc_page_ptr_sync(void);
void free_page_ptr_sync(void *page);

#endif
