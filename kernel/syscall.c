#include "syscall.h"
#include "kdebug.h"
#include <sys/syscall.h>
#include "initramfs.h"
#include "arch/paging.h"
#include "elf.h"

syscall_t syscall_table[256] = {
    [SYS_NR_EXEC] = sys_exec,
    [SYS_NR_DB_PRINT] = sys_db_print
};

size_t sys_db_print(const char *str) {
    kprintf("sys_db_print: \"%s\"\n", str);
    return 4;
}

size_t sys_exec(const char *path) {
    //TODO PROCESSES
    addr_space_t as = create_proc_addr_space();
    use_addr_space(as);
    const void *file = initramfs_getptr(path);
    kassert(file != NULL);

    uintptr_t entry = load_elf_file(file);
    asm ("jmp *%0"::"r" (entry));
    // should be unreachable
    return 4;
}
