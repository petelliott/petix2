#include "mem.h"
#include "arch/paging.h"
#include <limits.h>
#include <string.h>
#include <stdbool.h>

static uintptr_t mem_base;
static uintptr_t breakptr;

static size_t nframes;
static size_t sizeof_frames;
static size_t *frames;

static const size_t FREE_STACK_SIZE = 128;
static page_t free_stack[128];
static size_t free_stack_top;

#define CIEL(x, y) (((x) + (y) - 1)/(y))

void mem_init(uintptr_t base, uintptr_t initbrk, uintptr_t length) {
    mem_base = base;
    breakptr = initbrk;

    sizeof_frames = sizeof(size_t) * CIEL(length/PAGE_SIZE,
                                          CHAR_BIT*sizeof(size_t));
    // odds are this is aligned
    frames = ksbrk(sizeof_frames);
    nframes = length/PAGE_SIZE;

    memset(frames, 0, sizeof_frames);
    free_stack_top = 0;

    init_paging();
}

int kbrk(void *addr) {
    breakptr = (uintptr_t) addr;
    return 0;
}

void *ksbrk(intptr_t increment) {
    void *oldbrk = (void *) breakptr;
    breakptr += increment;
    return oldbrk;
}

static bool get_bit(size_t *array, size_t i) {
    return array[i/(sizeof(size_t)*CHAR_BIT)]
        & (1lu << (i % (sizeof(size_t)*CHAR_BIT)));
}

static void set_bit(size_t *array, size_t i, bool val) {
    if (val) {
        array[i/(sizeof(size_t)*CHAR_BIT)] |=
            (1lu << (i % (sizeof(size_t)*CHAR_BIT)));
    } else {
        array[i/(sizeof(size_t)*CHAR_BIT)] &=
            ~(1lu << (i % (sizeof(size_t)*CHAR_BIT)));
    }
}

page_t alloc_page(void) {
    if (free_stack_top > 0) {
        page_t frame = free_stack[--free_stack_top];
        set_bit(frames, frame - mem_base/PAGE_SIZE, 1);
        return frame;
    }

    for (size_t i = nframes/(sizeof(size_t)*CHAR_BIT); i >= 0; --i) {
        if (frames[i] != (size_t)-1) {
            size_t off = i*sizeof(size_t)*CHAR_BIT;
            size_t bit = __builtin_ctzl(~frames[i]);
            set_bit(frames, off + bit, true);
            return off + bit + mem_base/PAGE_SIZE;
        }
    }
}

void free_page(page_t page) {
    if (free_stack_top < FREE_STACK_SIZE) {
        free_stack[free_stack_top++] = page;
    }
    set_bit(frames, page-mem_base/PAGE_SIZE, false);
}

void *alloc_page_ptr(void) {
    return (void *) (alloc_page() << 12);
}

void free_page_ptr(void *page) {
    free_page(((uintptr_t) page) >> 12);
}
