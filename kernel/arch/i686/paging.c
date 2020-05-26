#include "../paging.h"
#include "mmu.h"
#include "interrupts.h"
#include "../../kdebug.h"
#include "../../mem.h"
#include <stddef.h>
#include <string.h>

#define PROC_REGION 0xC0000000

static page_dir_t kpagedir;

#define identity_len ((PROC_REGION/PAGE_SIZE)/PTAB_SIZE)

static page_tab_t page_tabs[identity_len];

static void page_fault_handler(struct pushed_regs *regs);

void init_paging(void) {
    kassert(sizeof(struct page_tab_ent) == 4);
    kassert(sizeof(struct page_dir_ent) == 4);
    kassert((((uint32_t) kpagedir.ents) & 0xfff) == 0);
    kassert((((uint32_t) page_tabs) & 0xfff) == 0);

    memset(&kpagedir, 0, sizeof(kpagedir));
    memset(&page_tabs, 0, sizeof(page_tabs));

    // identity page up to 0xc000000
    for (size_t i = 0; i < identity_len; ++i) {
        kpagedir.ents[i].present    = 1;
        kpagedir.ents[i].rw         = 1;
        kpagedir.ents[i].page_table = ((uint32_t) page_tabs[i].ents) >> PAGE_SHIFT;

        for (size_t j = 0; j < PTAB_SIZE; ++j) {
            struct page_tab_ent *pte = &(page_tabs[i].ents[j]);
            pte->present = 1;
            pte->rw = 1;
            pte->addr = (i*(PAGE_SIZE*PTAB_SIZE) + j*PAGE_SIZE) >> PAGE_SHIFT;
        }
    }

    register_interrupt_handler(14, page_fault_handler);

    load_page_dir(kpagedir.ents);
    enable_paging();

    kprintf("paging initialized\n");
}


static void page_fault_handler(struct pushed_regs *regs) {
    struct page_dir_ent *pd = get_page_dir();
    uintptr_t linaddr;
    asm ("mov %%cr2, %0": "=r" (linaddr));

    if ((regs->error_code & 0x9) != 0) {
        kprintf("unrecoverable page fault. pfla=%lx, ec=%lx\n",
                linaddr, regs->error_code);
        panic("unrecoverable page fault");
    }

    uint32_t dir_idx, tab_idx, offset;
    split_addr(linaddr, dir_idx, tab_idx, offset);

    if (!pd[dir_idx].present && !pd[dir_idx].petix_alloc) {
        kprintf("this should be a segfault. pfla=%lx, ec=%lx\n",
                linaddr, regs->error_code);
        panic("unrecoverable page fault");
    }

    // allocate a page table if needed
    if (!pd[dir_idx].present && pd[dir_idx].petix_alloc) {
        struct page_tab_ent *tab = alloc_page_ptr();
        memset(tab, 0, PAGE_SIZE);
        for (size_t i = 0; i < PTAB_SIZE; ++i) {
            tab[i].petix_alloc = 1;
            tab[i].petix_alloc = 1;
        }
        pd[dir_idx].present = 1;
        pd[dir_idx].page_table = (uint32_t) tab >> PAGE_SHIFT;
    }

    // allocate a page if needed

    kassert(pd[dir_idx].present);

    struct page_tab_ent *tab = (void *) (pd[dir_idx].page_table << 12);

    if (!tab[tab_idx].present && !tab[tab_idx].petix_alloc) {
        kprintf("this should be a segfault. pfla=%lx, ec=%lx\n",
                linaddr, regs->error_code);
        panic("unrecoverable page fault");
    }

    if (!tab[tab_idx].present && tab[tab_idx].petix_alloc) {
        void *page = alloc_page_ptr();
        memset(page, 0, PAGE_SIZE);
        tab[tab_idx].present = 1;
        tab[tab_idx].addr = (uint32_t) page >> PAGE_SHIFT;
    }

}

addr_space_t create_proc_addr_space(void) {
    struct page_dir_ent *pd = alloc_page_ptr();
    memcpy(pd, kpagedir.ents, PAGE_SIZE);

    // null initialized pages above to 0xc000000
    for (size_t i = identity_len; i < PDIR_SIZE; ++i) {
        pd[i].present        = 0;
        pd[i].petix_alloc    = 1;
    }

    return pd;
}

void free_proc_addr_space(addr_space_t as) {
    for (size_t i = identity_len; i < PDIR_SIZE; ++i) {
        if (as[i].present && as[i].petix_alloc) {
            struct page_tab_ent *pte = (void *) (as[i].page_table << PAGE_SHIFT);
            for (size_t j = 0; j < PTAB_SIZE; ++j) {
                free_page(pte[i].addr);
            }

            free_page(as[i].page_table);
        }
    }
    free_page_ptr(as);
}

void fork_proc_addr_space(addr_space_t as);

void use_addr_space(addr_space_t as) {
    load_page_dir(as);
}
