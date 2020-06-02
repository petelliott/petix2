#include "syscall.h"
#include "kdebug.h"
#include <sys/syscall.h>
#include "arch/paging.h"
#include "arch/switch.h"
#include "elf.h"
#include "proc.h"
#include "sync.h"
#include "fs.h"
#include "kmalloc.h"
#include <errno.h>
#include <string.h>

syscall_t syscall_table[256] = {
    [SYS_NR_READ] = sys_read,
    [SYS_NR_WRITE] = sys_write,
    [SYS_NR_OPEN] = sys_open,
    [SYS_NR_CLOSE] = sys_close,
    [SYS_NR_WAITPID] = sys_waitpid,
    [SYS_NR_SCHED_YIELD] = sys_sched_yield,
    [SYS_NR_FORK] = sys_fork,
    [SYS_NR_EXEC] = sys_exec,
    [SYS_NR_EXIT] = sys_exit,
    [SYS_NR_DB_PRINT] = sys_db_print
};

ssize_t sys_read(size_t fd, char *buf, size_t count) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || !pcb->fds[fd].valid) {
        return -EBADF;
    }

    struct file *f = &(pcb->fds[fd].file);

    if (f->fops->read == NULL) {
        return -EPERM;
    }

    return f->fops->read(f, buf, count);
}

ssize_t sys_write(size_t fd, const char *buf, size_t count) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || !pcb->fds[fd].valid) {
        return -EBADF;
    }

    struct file *f = &(pcb->fds[fd].file);

    if (f->fops->write == NULL) {
        return -EPERM;
    }

    return f->fops->write(f, buf, count);
}

ssize_t sys_open(const char *path, int flags, int mode) {
    ssize_t err;
    struct pcb *pcb = get_pcb(get_pid());

    struct inode in;
    err = fs_lookup(path, &in);
    if (err < 0) {
        return err;
    }

    int fd = alloc_fd(pcb);
    if (fd < 0) {
        return fd;
    }

    err = fs_open(&in, &(pcb->fds[fd].file));
    if (err < 0) {
        release_fd(pcb, fd);
        return err;
    }

    return fd;
}

ssize_t sys_close(size_t fd) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || !pcb->fds[fd].valid) {
        return -EBADF;
    }

    struct file *f = &(pcb->fds[fd].file);
    release_fd(pcb, fd);

    if (f->fops->close != NULL) {
        return f->fops->close(f);
    } else {
        return 0;
    }
}


ssize_t sys_waitpid(pid_t pid, int *wstatus, int options) {
    struct pcb *pcb = get_pcb(get_pid());

    int ret = proc_get_terminated_child(pcb, pid);
    if (ret == 0 || ret == -1) {
        if (ret == -1) {
            acquire_global();

            pcb->rs = RS_BLOCKED;
            pcb->wait_pid = pid;

            release_global();
            sched();
        }
        //there is a currently terminated process
        acquire_global();
        struct pcb *cpcb = get_pcb(pcb->wait_pid);

        *wstatus = cpcb->return_code;
        // destroy proc
        cpcb->rs = RS_NOPROC;
        cpcb->ppid = 0;
        pid_t pid = pcb->wait_pid;
        pcb->wait_pid = NOT_WAITING;

        release_global();
        return pid;
    } else {
        return ret;
    }
}

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

    if (new == NULL) {
        return -EAGAIN;
    }

    acquire_global();
    new->ppid = old->pid;
    new->rs = RS_READY;
    memcpy(new->fds, old->fds, sizeof(new->fds));

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
    acquire_global();

    struct pcb *pcb = get_pcb(get_pid());
    pcb->return_code = code;

    struct pcb *ppcb = get_pcb(pcb->ppid);

    // wake waiting process if it exists
    if (ppcb->rs == RS_BLOCKED && (ppcb->wait_pid == pcb->pid ||
                                   ppcb->wait_pid == -1)) {

        ppcb->wait_pid = pcb->pid;
        ppcb->rs = RS_READY;
    }

    pcb->rs = RS_TERMINATED;
    free_proc_addr_space(pcb->addr_space);

    release_global();

    sched();
    //should be unreachable
    return 4;
}
