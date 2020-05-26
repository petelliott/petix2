#include "syscall.h"
#include "kdebug.h"
#include <sys/syscall.h>
#include "initramfs.h"
#include "arch/paging.h"
#include "elf.h"
#include "proc.h"

syscall_t syscall_table[256] = {
    [SYS_NR_EXEC] = sys_exec,
    [SYS_NR_EXIT] = sys_exit,
    [SYS_NR_DB_PRINT] = sys_db_print
};

size_t sys_db_print(const char *str) {
    kprintf("sys_db_print: \"%s\"\n", str);
    return 4;
}

size_t sys_exec(const char *path) {
    //TODO PROCESSES

    struct pcb *pcb = get_pcb(get_pid());
    free_proc_addr_space(pcb->addr_space);
    pcb->addr_space = create_proc_addr_space();
    use_addr_space(pcb->addr_space);

    const void *file = initramfs_getptr(path);
    kassert(file != NULL);

    uintptr_t entry = load_elf_file(file);

    asm volatile ("mov 0xffffffff, %%esp\n"
                  "jmp *%0\n"
                  ::"r" (entry));
    // should be unreachable
    return 4;
}

size_t sys_exit(size_t code) {
    struct pcb *pcb = get_pcb(get_pid());
    pcb->return_code = code;
    pcb->rs = RS_TERMINATED;
    sched();

    //should be unreachable
    return 4;
}
