#include "tty.h"
#include "multiboot.h"
#include "tar.h"
#include <stddef.h>
#include <string.h>


void kmain(unsigned long magic, unsigned long addr) {
	term_clear();

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        error("invalid magic number: ");
        printx(magic);
        print("\n");
    } else {
        print("correct multiboot magic number!\n");
    }

    multiboot_info_t *mbi = (multiboot_info_t *) addr;
    print("multiboot info at "); printx(addr); print("\n");

    multiboot_module_t *mods = (multiboot_module_t *) mbi->mods_addr;

    for (int i = 0; i < mbi->mods_count; ++i) {
        print("found module '");
        print((const char *) mods[i].cmdline);
        print("' at ");
        printx(mods[i].mod_start);
        print("...");
        printx(mods[i].mod_end);
        print("\n");

        if (strcmp((const char *)mods[i].cmdline, "initrd") == 0) {
            print("initializing initramfs\n");
            //TODO: load initrd based on name
        }
    }

    for (struct tar *tar = (void *) mods[0].mod_start;
         tar != NULL; tar = tar_next(tar)) {

        print(tar->name);
        print("\n");
        print(tar_contents(tar));
    }

    multiboot_memory_map_t *mems = (multiboot_memory_map_t *) mbi->mmap_addr;
    multiboot_memory_map_t *mend = ((void *) mems) + mbi->mmap_length;

    multiboot_memory_map_t *m;
    for (m = mems; m < mend; m = ((void *) m) + m->size + 4) {
        if (m->type == MULTIBOOT_MEMORY_AVAILABLE) {
            print("available memory: ");
        } else if (m->type == MULTIBOOT_MEMORY_RESERVED) {
            print("reserved memory : ");
        } else if (m->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
            print("acpi rec memory : ");
        } else if (m->type == MULTIBOOT_MEMORY_NVS) {
            print("NVS memory      : ");
        } else if (m->type == MULTIBOOT_MEMORY_BADRAM) {
            print("bad memory      : ");
        }
        printx(m->addr);
        print("...");
        printx(m->addr + m->len);
        print("\n");
    }
}
