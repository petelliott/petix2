#include "tty.h"
#include "multiboot.h"
#include "tar.h"
#include <stddef.h>


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

        //TODO: load initrd based on name
    }

    for (struct tar *tar = (void *) mods[0].mod_start;
         tar != NULL; tar = tar_next(tar)) {

        print(tar->name);
        print("\n");
        print(tar_contents(tar));
    }

}
