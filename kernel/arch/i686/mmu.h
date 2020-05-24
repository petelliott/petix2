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
    uint32_t ignored       : 4;
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
    uint32_t ignored       : 3;
    uint32_t addr          : 20;
};

typedef struct page_dir_ent page_dir_t[1024];
typedef struct page_tab_ent page_tab_t[1024];

void *virt_to_phys(const void *virt);

/* loads a memory map and flushes tlb*/
void load_page_dir(struct page_dir_ent *dir);

void enable_paging(void);

#endif
