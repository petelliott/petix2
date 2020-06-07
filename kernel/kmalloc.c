// most of this file is ported from plibc
#include "kmalloc.h"

#include <stdint.h>
#include <string.h>
#include "mem.h"

#define MALLOC_MAG 0x8a05e5ad623cc4e
#define MIN_SPLIT_SIZE 128 //must be larger than 40 or sizeof(struct block)+8

struct block {
    struct block *prev;
    struct block *next;
    uint64_t pos_mag; // position dependant magic number
    uint32_t len;     // length of the block
    uint32_t used;    // this could be one byte but we need to preserve 8 byte alignment
};

static struct block *base = NULL;

#define align8(i) ((8 - (((uintptr_t) i) % 8)) % 8)


/*
    expands the data segment and inserts a new block of size
    used when no freed block can be found
*/
static struct block *push_block(size_t size) {
    void *start = ksbrk(0);
    // we need to make sure that our blocks are aligned.
    // the malloc definion guarantees that the pointer is aligned for
    // any built in type.
    // 8 bytes on 64 bit systems
    long align = align8(start);

    long alloc_size = align + sizeof(struct block) + size;
    ksbrk(alloc_size);

    struct block *new_block = (struct block *)(((char *)start) + align);

    if (base != NULL) {
        base->next = new_block;
    }

    new_block->prev    = base;
    new_block->next    = NULL;
    new_block->pos_mag = MALLOC_MAG ^ ((long) new_block);
    new_block->len     = size;
    new_block->used    = 1;

    base = new_block;

    return new_block;
}


/*
    splits a given block in two if possible,
    the first one being size.
    returns 1 if split 0 if not
*/
static int shrink_block(struct block *mblock, size_t size) {
    if (mblock->len - size >= MIN_SPLIT_SIZE) {
        struct block *block_new = (struct block *)((char *)(mblock+1) + size);
        block_new = (struct block *)((char *)block_new + align8(block_new));

        block_new->prev    = mblock;
        block_new->next    = mblock->next;
        block_new->pos_mag = MALLOC_MAG ^ ((long) block_new);
        block_new->len     = (uintptr_t)mblock + mblock->len - (uintptr_t)block_new;
        block_new->used    = 0;

        if (mblock == base) {
            base = block_new;
        }

        if (mblock->next != NULL) {
            mblock->next->prev = block_new;
        }
        mblock->next = block_new;
        mblock->len = size;
        return 1;
    }
    return 0;
}


/*
    finds a free block of at least size.
*/
static struct block *get_free_block(size_t size) {
    struct block *curr = base;
    while (curr != NULL) {
        if (!curr->used && curr->len >= size) {
            shrink_block(curr, size);
            curr->used = 1;
            return curr;
        }
        curr = curr->prev;
    }
    return NULL;
}


/*
    merges a block its next block.
    should only be called when both blocks are free
*/
static void block_merge_next(struct block *mblock) {
    // accounting for padding between end of one block and the start of another
    mblock->len = (uintptr_t)mblock->next - (uintptr_t)mblock + mblock->next->len;

    if (mblock->next == base) {
        base = mblock;
    }

    mblock->next = mblock->next->next;
    if (mblock->next != NULL) {
        mblock->next->prev = mblock;
    }
}


/*
    determines, by the magic number, if a block is valid
*/
static int is_mem_block(struct block *mblock) {
    return (((long) mblock) ^ mblock->pos_mag) == MALLOC_MAG;
}

void *kmalloc(size_t size) {
    struct block* mblock;

    if (size == 0) {
        return NULL;
    }

    if (base == NULL) {
        mblock = push_block(size);
        return mblock + 1; // skip to the useable memory
    }

    mblock = get_free_block(size);
    if (mblock == NULL) {
        mblock = push_block(size);
    }
    return mblock + 1; // skip to the useable memory
}

void *krealloc(void *ptr, size_t size) {
    struct block *mblock = ((struct block *) ptr) -1;

    if (ptr == NULL) {
        return kmalloc(size);
    }

    if (!is_mem_block(mblock)) {
        return NULL;
    }

    if (size < mblock->len) {
        shrink_block(mblock, size);
        return ptr;
    } else if (mblock->next == NULL && mblock == base) {
        ksbrk(size - mblock->len);
        mblock->len = size;
        return ptr;
    } else if (mblock->next->used == 0 && (long)mblock->next -
               (long)mblock + mblock->next->len >= size) {
        block_merge_next(mblock);
        return ptr;
    } else {
        void *new_ptr = kmalloc(size);
        memcpy(new_ptr, ptr, mblock->len);
        kfree(ptr);
        return new_ptr;
    }
}

void kfree(void *ptr) {
    if (ptr == NULL) {
        // free is required to handle NULL pointers;
        return;
    }

    struct block *mblock = ((struct block *) ptr) -1;
    // check that ptr came from malloc()
    if (is_mem_block(mblock)) {
        mblock->used = 0;


        if (mblock->next != NULL && !mblock->next->used) {
            block_merge_next(mblock);
        }

        if (mblock->prev != NULL && !mblock->prev->used) {
            block_merge_next(mblock->prev);
        }
    }
}

void *kmalloc_sync(size_t size) {
    acquire_lock(&memlock);
    void *km = kmalloc(size);
    release_lock(&memlock);
    return km;
}

void *krealloc_sync(void *ptr, size_t size) {
    acquire_lock(&memlock);
    void *km = krealloc(ptr, size);
    release_lock(&memlock);
    return km;
}

void kfree_sync(void *ptr) {
    acquire_lock(&memlock);
    kfree(ptr);
    release_lock(&memlock);
}
