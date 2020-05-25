#include "mmu.h"
#include "../../kdebug.h"
#include <stddef.h>

void *virt_to_phys(const void *virt) {
    uint32_t addr = (uint32_t) virt;

    uint32_t offset  = addr & 0xfff;
    uint32_t tab_idx = (addr >> 12) & 0x003ff;
    uint32_t dir_idx = addr >> 22;

    struct page_dir_ent *pde;
    asm volatile ("mov %%cr3, %0"
                  : "=r" (pde));

    struct page_tab_ent pte = ((struct page_tab_ent *)(uint32_t)
                               pde[dir_idx].page_table)[tab_idx];
    if (pte.present) {
        return (void *)(uint32_t) ((pte.addr << 12) | offset);
    } else {
        return NULL;
    }
}

/* loads a memory map and flushes tlb */
void load_page_dir(struct page_dir_ent *dir) {
    uintptr_t val = (uintptr_t) dir;
    kassert((val & (4096 - 1)) == 0);
    asm volatile ("mov %0, %%cr3\n"
                  :
                  : "r" (dir));
}

void enable_paging(void) {
    asm volatile ("mov %%cr0, %%eax\n"
                  "or $0x80000000, %%eax\n"
                  "mov %%eax, %%cr0\n"
                  ::: "eax");
}
