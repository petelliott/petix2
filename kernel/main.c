#include "tty.h"
#include "multiboot.h"
#include "tar.h"
#include "initramfs.h"
#include "kdebug.h"
#include "mem.h"
#include "kmalloc.h"
#include "arch/cpu.h"
#include "arch/paging.h"
#include <stddef.h>
#include <string.h>



void kmain(unsigned long magic, unsigned long addr) {
	tty_init();
    init_cpu();
    kassert(magic == MULTIBOOT_BOOTLOADER_MAGIC);

    multiboot_info_t *mbi = (multiboot_info_t *) addr;

    multiboot_module_t *mods = (multiboot_module_t *) mbi->mods_addr;

    kassert(mbi->mods_count == 1);

    kprintf("found initramfs '%s' at %X...%X\n",
            (const char *) mods[0].cmdline,
            mods[0].mod_start, mods[0].mod_end);

    kprintf("initializing initramfs\n");
    initramfs_init((void *) mods[0].mod_start);

    kassert(initramfs_initialized());

    multiboot_memory_map_t *mems = (multiboot_memory_map_t *) mbi->mmap_addr;
    multiboot_memory_map_t *mend = ((void *) mems) + mbi->mmap_length;

    multiboot_memory_map_t *m;
    for (m = mems; m < mend; m = ((void *) m) + m->size + 4) {
        if (m->addr == 0x100000) {
            // we only use this region for now.

            //TODO: detect initbrk better.
            kprintf("initializing memory manager\n");
            kprintf("mem: %llX...%llX\n", m->addr, m->addr + m->len);
            mem_init(m->addr, mods[0].mod_end, m->len);
            kprintf("%lluMB free\n", m->len / (1024 * 1024));
            break;
        }
    }


    sti();

    /*
    kprintf("ret %li\n", raw_syscall(SYS_NR_DB_PRINT, "heyyyy"));

    addr_space_t as = create_proc_addr_space();
    use_addr_space(as);
    volatile uint32_t a = *(uint32_t *)0xc1000000;
    kprintf("eyyy: %lx\n", a);
    a = *(uint32_t *)0xc1000008;
    kprintf("eyyy: %lx\n", a);

    free_proc_addr_space(as);
    */

    char ch;
    while (1) {
        tty_read(&ch, 1);
        tty_write(&ch, 1);
    }

    for (;;) {
        halt();
    }
}
