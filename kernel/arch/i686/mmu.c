#include "mmu.h"
#include "../../kdebug.h"
#include <stddef.h>


struct page_dir_ent *get_page_dir(void) {
    struct page_dir_ent *pde;
    asm volatile ("mov %%cr3, %0"
                  : "=r" (pde));
    return pde;
}

void *virt_to_phys(const void *virt) {
    uint32_t addr = (uint32_t) virt;

    uint32_t dir_idx, tab_idx, offset;
    split_addr_o(addr, dir_idx, tab_idx, offset);

    struct page_dir_ent *pde = get_page_dir();

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

void enable_global_pages(void) {
    asm volatile ("mov %%cr4, %%eax\n"
                  "or $0x00000080, %%eax\n"
                  "mov %%eax, %%cr4\n"
                  ::: "eax");
}

void flush_tlb(void) {
    asm volatile ("    mov %%cr3, %%eax\n"
                  "    mov %%eax, %%cr3\n"
                  ::: "eax");
}
