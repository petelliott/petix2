#include "elf.h"
#include "kdebug.h"
#include <string.h>

uint8_t correct_e_ident[ELF_NIDENT] = {
    0x7f, 'E', 'L', 'F',
    [EI_CLASS]      = 1,
    [EI_DATA]       = 1,
    [EI_VERSION]    = 1,
    [EI_OSABI]      = 0x00,
    [EI_ABIVERSION] = 0, 0
};

bool check_elf_header(const Elf32_Ehdr *hdr) {
    return memcmp(hdr->e_ident, correct_e_ident, ELF_NIDENT) == 0;
}

uintptr_t load_elf_file(const void *file) {
    const Elf32_Ehdr *hdr = file;

    //TODO this shouldn't be a kassert
    kassert(check_elf_header(hdr));
    kassert(hdr->e_type == ET_EXEC);

    const Elf32_Phdr *phdrs = (file + hdr->e_phoff);

    for (size_t i = 0; i < hdr->e_phnum; ++i) {
        if (phdrs[i].p_type == PT_LOAD) {
            kassert(phdrs[i].p_vaddr >= 0xc0000000);

            memcpy((void *) phdrs[i].p_vaddr,
                   file + phdrs[i].p_offset,
                   phdrs[i].p_filesz);
        }
    }

    return hdr->e_entry;
}
