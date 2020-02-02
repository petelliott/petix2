#include "tty.h"
#include "multiboot.h"
#include "tar.h"
#include "kdebug.h"
#include <stddef.h>
#include <string.h>


void kmain(unsigned long magic, unsigned long addr) {
	tty_init();

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        kprintf("error: invalid magic number: %lX\n", magic);
    } else {
        kprintf("correct multiboot magic number!\n");
    }

    multiboot_info_t *mbi = (multiboot_info_t *) addr;
    kprintf("multiboot info at %lX\n", addr);

    multiboot_module_t *mods = (multiboot_module_t *) mbi->mods_addr;

    for (int i = 0; i < mbi->mods_count; ++i) {
        kprintf("found module '%s' at %X...%X\n",
                (const char *) mods[i].cmdline,
                mods[i].mod_start, mods[i].mod_end);

        if (strcmp((const char *)mods[i].cmdline, "initrd") == 0) {
            kprintf("initializing initramfs\n");
            //TODO: setup initramfs
        }
    }

    /*
    for (struct tar *tar = (void *) mods[0].mod_start;
         tar != NULL; tar = tar_next(tar)) {

        print(tar->name);
        print("\n");
        print(tar_contents(tar));
    }
    */

    multiboot_memory_map_t *mems = (multiboot_memory_map_t *) mbi->mmap_addr;
    multiboot_memory_map_t *mend = ((void *) mems) + mbi->mmap_length;

    multiboot_memory_map_t *m;
    for (m = mems; m < mend; m = ((void *) m) + m->size + 4) {
        if (m->type == MULTIBOOT_MEMORY_AVAILABLE) {
            kprintf("available memory: ");
        } else if (m->type == MULTIBOOT_MEMORY_RESERVED) {
            kprintf("reserved memory : ");
        } else if (m->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
            kprintf("acpi rec memory : ");
        } else if (m->type == MULTIBOOT_MEMORY_NVS) {
            kprintf("NVS memory      : ");
        } else if (m->type == MULTIBOOT_MEMORY_BADRAM) {
            kprintf("bad memory      : ");
        }
        kprintf("%llX...%llX\n", m->addr, m->addr + m->len);
    }
}
