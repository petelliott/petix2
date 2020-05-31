#include "syscall.h"
#include "kdebug.h"
#include <sys/syscall.h>
#include "initramfs.h"
#include "arch/paging.h"
#include "arch/switch.h"
#include "elf.h"
#include "proc.h"
#include "sync.h"
#include "fs.h"
#include "kmalloc.h"
#include <errno.h>

syscall_t syscall_table[256] = {
    [SYS_NR_SCHED_YIELD] = sys_sched_yield,
    [SYS_NR_FORK] = sys_fork,
    [SYS_NR_EXEC] = sys_exec,
    [SYS_NR_EXIT] = sys_exit,
    [SYS_NR_DB_PRINT] = sys_db_print
};

ssize_t sys_db_print(const char *str) {
    kprintf("sys_db_print (pid=%li): \"%s\"\n", get_pid(), str);
    return 4;
}

ssize_t sys_sched_yield(void) {
    sched();
    return 0;
}

ssize_t sys_fork(void) {
    struct pcb *old = get_pcb(get_pid());
    struct pcb *new = alloc_proc();

    acquire_global();
    new->ppid = old->pid;
    new->rs = RS_READY;

    fork_switchable(&(new->stack_ptr), old->addr_space, &(new->addr_space));

    if (get_pid() == new->pid) {
        release_global();
        return 0;
    } else {
        release_global();
        return new->pid;
    }
}

ssize_t sys_exec(const char *path) {
    int err;

    struct pcb *pcb = get_pcb(get_pid());
    free_proc_addr_space(pcb->addr_space);
    pcb->addr_space = create_proc_addr_space();
    use_addr_space(pcb->addr_space);

    //TODO: this is the worst way to set up a stack
    volatile char b;
    b = *(char *) 0xfffff000;
    b = *(char *) 0xffffe000;
    b = *(char *) 0xffffd000;
    b = *(char *) 0xffffc000;

    // using the internal file api, because we don't want to waste an
    // fd
    struct inode in;
    struct file f;
    err = fs_lookup(path, &in);
    if (err < 0) {
        return err;
    }
    err = fs_open(&in, &f);
    if (err < 0) {
        return err;
    }

    if (f.fops->read == NULL) {
        return -EPERM;
    }

    void *data = kmalloc_sync(f.size);
    err = f.fops->read(&f, data, f.size);
    if (err < 0) {
        return err;
    }
    if (f.fops->close != NULL) {
        f.fops->close(&f);
    }

    uintptr_t entry = load_elf_file(data);

    kfree_sync(data);

    //TODO make this more portable
    asm volatile ("mov $0xfffffff0, %%esp\n"
                  "jmp *%0\n"
                  ::"r" (entry));
    // should be unreachable
    return 4;
}

ssize_t sys_exit(size_t code) {
    struct pcb *pcb = get_pcb(get_pid());
    pcb->return_code = code;
    pcb->rs = RS_TERMINATED;
    free_proc_addr_space(pcb->addr_space);
    sched();

    //should be unreachable
    return 4;
}
