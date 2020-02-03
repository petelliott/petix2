#include "tty.h"
#include "multiboot.h"
#include "tar.h"
#include "initramfs.h"
#include "kdebug.h"
#include "mem.h"
#include <stddef.h>
#include <string.h>


void kmain(unsigned long magic, unsigned long addr) {
	tty_init();
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

    /*
    kprintf("reading motd from initramfs\n");
    int fd = initramfs_open("etc/motd");
    kassert(fd != -1);
    char buf[5];
    size_t nr;
    while ((nr = initramfs_read(fd, buf, 5)) != 0) {
        tty_write(buf, nr);
    }
    kprintf("\n");
    initramfs_close(fd);
    */

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
}
