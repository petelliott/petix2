#include "../paging.h"
#include "mmu.h"
#include "../../kdebug.h"
#include <stddef.h>
#include <string.h>

#define PROC_REGION 0xC0000000

static page_dir_t kpagedir;

#define identity_len ((PROC_REGION/PAGE_SIZE)/PTAB_SIZE)

static page_tab_t page_tabs[identity_len];

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

    load_page_dir(kpagedir.ents);
    enable_paging();

    kprintf("paging initialized\n");
}

addr_space_t create_proc_addr_space(void);
void free_proc_addr_space(addr_space_t as);

void fork_proc_addr_space(addr_space_t as);
