#include "tty.h"
#include "multiboot.h"
#include "tar.h"
#include "initramfs.h"
#include "kdebug.h"
#include <stddef.h>
#include <string.h>


void kmain(unsigned long magic, unsigned long addr) {
	tty_init();
    kassert(magic == MULTIBOOT_BOOTLOADER_MAGIC);

    multiboot_info_t *mbi = (multiboot_info_t *) addr;

    multiboot_module_t *mods = (multiboot_module_t *) mbi->mods_addr;

    for (int i = 0; i < mbi->mods_count; ++i) {
        kprintf("found module '%s' at %X...%X\n",
                (const char *) mods[i].cmdline,
                mods[i].mod_start, mods[i].mod_end);

        if (strcmp((const char *)mods[i].cmdline, "initrd") == 0) {
            kprintf("initializing initramfs\n");
            initramfs_init((void *) mods[i].mod_start);
        }
    }

    kassert(initramfs_initialized());

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

    multiboot_memory_map_t *mems = (multiboot_memory_map_t *) mbi->mmap_addr;
    multiboot_memory_map_t *mend = ((void *) mems) + mbi->mmap_length;

    multiboot_memory_map_t *m;
    for (m = mems; m < mend; m = ((void *) m) + m->size + 4) {
        if (m->type == MULTIBOOT_MEMORY_AVAILABLE) {
            kprintf("available memory: ");
        } else if (m->type == MULTIBOOT_MEMORY_RESERVED) {
            kprintf("reserved  memory: ");
        } else if (m->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
            kprintf("acpi rec  memory: ");
        } else if (m->type == MULTIBOOT_MEMORY_NVS) {
            kprintf("NVS memory      : ");
        } else if (m->type == MULTIBOOT_MEMORY_BADRAM) {
            kprintf("bad memory      : ");
        }
        kprintf("%llX...%llX\n", m->addr, m->addr + m->len);
    }
}
