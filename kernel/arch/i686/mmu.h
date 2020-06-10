#ifndef MMU_H
#define MMU_H

#include <stdint.h>

struct page_dir_ent {
    uint32_t present       : 1;
    uint32_t rw            : 1;
    uint32_t user          : 1;
    uint32_t write_through : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed      : 1;
    uint32_t zero          : 1;
    uint32_t size          : 1;
    uint32_t petix_alloc   : 1; // allocate a page table if we fault on this
    uint32_t ignored       : 3;
    uint32_t page_table    : 20;
};

struct page_tab_ent {
    uint32_t present       : 1;
    uint32_t rw            : 1;
    uint32_t user          : 1;
    uint32_t write_through : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed      : 1;
    uint32_t dirty         : 1;
    uint32_t zero          : 1;
    uint32_t global        : 1;
    uint32_t petix_alloc   : 1; // allocate a page if we fault on this
    uint32_t ignored       : 2;
    uint32_t addr          : 20;
};

#define PDIR_SIZE 1024
#define PTAB_SIZE 1024

#define PAGE_SIZE 4096
#define PAGE_MASK 0xfff
#define PAGE_SHIFT 12

#define split_addr_o(addr,d,t,o) {              \
        o  = (addr) & 0xfff;                    \
        t = ((addr) >> 12) & 0x003ff;           \
        d = (addr) >> 22;                       \
    }

#define split_addr(addr,d,t) {                  \
        t = ((addr) >> 12) & 0x003ff;           \
        d = (addr) >> 22;                       \
    }

typedef struct {
    struct page_dir_ent ents[1024];
} __attribute__((aligned(4096))) page_dir_t;

typedef struct {
    struct page_tab_ent ents[1024];
} __attribute__((aligned(4096))) page_tab_t;

struct page_dir_ent *get_page_dir(void);
void *virt_to_phys(const void *virt);

/* loads a memory map and flushes tlb*/
void load_page_dir(struct page_dir_ent *dir);

void enable_paging(void);

#endif
