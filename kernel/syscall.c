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
#include "pipe.h"
#include <errno.h>
#include <string.h>
#include <fcntl.h>


//TODO
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"

syscall_t syscall_table[256] = {
    [SYS_NR_READ]    = sys_read,
    [SYS_NR_WRITE]   = sys_write,
    [SYS_NR_OPEN]    = sys_open,
    [SYS_NR_CLOSE]   = sys_close,
    [SYS_NR_WAITPID] = sys_waitpid,
    [SYS_NR_DUP2]    = sys_dup2,
    [SYS_NR_GETDENT] = sys_getdent,

    [SYS_NR_IOCTL]   = sys_ioctl,

    [SYS_NR_PIPE]    = sys_pipe,
    [SYS_NR_SCHED_YIELD] = sys_sched_yield,
    [SYS_NR_FORK]     = sys_fork,
    [SYS_NR_EXEC]     = sys_exec,
    [SYS_NR_EXIT]     = sys_exit,
    [SYS_NR_DB_PRINT] = sys_db_print
};

#pragma GCC diagnostic pop

ssize_t sys_read(ssize_t fd, char *buf, size_t count) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || pcb->fds[fd].file == NULL) {
        return -EBADF;
    }

    struct file *f = pcb->fds[fd].file;

    if (f->fops->read == NULL) {
        return -EPERM;
    }

    return f->fops->read(f, buf, count);
}

ssize_t sys_write(ssize_t fd, const char *buf, size_t count) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || pcb->fds[fd].file == NULL) {
        return -EBADF;
    }

    struct file *f = pcb->fds[fd].file;

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

    if (flags & O_CLOEXEC) {
        pcb->fds[fd].cloexec = true;
    }

    err = fs_open(&in, pcb->fds[fd].file, flags);
    if (err < 0) {
        release_fd(pcb, fd);
        return err;
    }

    return fd;
}

ssize_t sys_close(ssize_t fd) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || pcb->fds[fd].file == NULL) {
        return -EBADF;
    }

    release_fd(pcb, fd);
    return 0;
}

ssize_t sys_dup2(ssize_t fd, ssize_t fd2) {
    struct pcb *pcb = get_pcb(get_pid());
    if (fd >= MAX_FDS || fd < 0 || pcb->fds[fd].file == NULL) {
        return -EBADF;
    }

    if (fd2 >= MAX_FDS || fd2 < 0) {
        return -EBADF;
    }

    if (fd == fd2) {
        return fd;
    }

    if (pcb->fds[fd2].file != NULL) {
        sys_close(fd2);
    }

    dup_fd(pcb, fd);
    pcb->fds[fd2].file = pcb->fds[fd].file;
    return fd2;
}

ssize_t sys_getdent(ssize_t fd, struct petix_dirent *dent) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || pcb->fds[fd].file == NULL) {
        return -EBADF;
    }

    struct file *f = pcb->fds[fd].file;

    if (f->inode.ftype != FT_DIR) {
        return -ENOTDIR;
    }

    if (f->fops->getdent == NULL) {
        return -EPERM;
    }

    return f->fops->getdent(f, dent);
}

ssize_t sys_pipe(int filedes[2], size_t flags) {
    struct pcb *pcb = get_pcb(get_pid());

    int fd1 = alloc_fd(pcb);
    if (fd1 < 0) {
        return fd1;
    }

    int fd2 = alloc_fd(pcb);
    if (fd2 < 0) {
        return fd2;
    }

    open_pipe(pcb->fds[fd1].file, pcb->fds[fd2].file);
    if (flags & O_CLOEXEC) {
        pcb->fds[fd1].cloexec = true;
        pcb->fds[fd2].cloexec = true;
    }

    filedes[0] = fd1;
    filedes[1] = fd2;

    return 0;
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

ssize_t sys_ioctl(ssize_t fd, size_t req, ...) {
    struct pcb *pcb = get_pcb(get_pid());

    if (fd >= MAX_FDS || fd < 0 || pcb->fds[fd].file == NULL) {
        return -EBADF;
    }

    struct file *f = pcb->fds[fd].file;

    if (f->fops->ioctl == NULL) {
        return -ENOTTY;
    }

    va_list ap;
    va_start(ap, req);
    return f->fops->ioctl(f, req, ap);
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

    for (size_t i = 0; i < MAX_FDS; ++i) {
        if (new->fds[i].file != NULL) {
            dup_fd(new, i);
        }
    }

    fork_switchable(&(new->stack_ptr), old->addr_space, &(new->addr_space));

    if (get_pid() == new->pid) {
        release_global();
        return 0;
    } else {
        release_global();
        return new->pid;
    }
}

ssize_t sys_exec(const char *path, char *const argv[], char *const envp[]) {
    int err;

    //TODO: this is the worst way to set up a stack
    volatile char b;
    b = *(char *) 0xfffff000;
    b = *(char *) 0xffffe000;
    b = *(char *) 0xffffd000;
    b = *(char *) 0xffffc000;
    b = *(char *) 0xffffb000;
    b = *(char *) 0xffffa000;
    b = *(char *) 0xffff9000;
    b = *(char *) 0xffff8000;

    // prevent -Wunused-but-set and -Wset-but-unused
    b = b;

    // close O_CLOEXEC fds
    struct pcb *pcb = get_pcb(get_pid());

    for (size_t i = 0; i < MAX_FDS; ++i) {
        if (pcb->fds[i].file != NULL && pcb->fds[i].cloexec) {
            release_fd(pcb, i);
        }
    }

    if (path == NULL) {
        return -EINVAL;
    }

    // using the internal file api, because we don't want to waste an
    // fd
    struct inode in;
    struct file f;
    err = fs_lookup(path, &in);
    if (err < 0) {
        return err;
    }

    if (!in.exec) {
        return -EACCES;
    }

    err = fs_open(&in, &f, 0);
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

    char *cdata = data;
    if (cdata[0] == '#' && cdata[1] == '!') {
        //TODO dont read the whole file to check this
        char *save;
        char *script = strtok_r(cdata+2, " \n\t", &save);
        char scbuff[PATH_MAX];
        strncpy(scbuff, script, PATH_MAX);
        kfree_sync(data);

        char *newargv[128] = {NULL};
        newargv[0] = scbuff;
        for (size_t i = 0; i < 128 && argv !=NULL && argv[i] != NULL; ++i) {
            newargv[i+1] = argv[i];
        }
        return sys_exec(scbuff, newargv, envp);
    }

    if (!check_elf_header(data)) {
        return -ENOEXEC;
    }


    // push args;
    size_t argc = 0;
    if (argv != NULL) {
        for (argc = 0; argv[argc] != NULL; ++argc) {}
    }

    uintptr_t sp = 0xfffffff0;

    size_t argv_size = argc*sizeof(char *);
    char **tmp_argv = kmalloc_sync(argv_size);


    for (size_t i = 0; i < argc; ++i) {
        size_t len = strlen(argv[i]) + 1;
        sp -= len;
        memcpy((void *)sp, argv[i], len);
        tmp_argv[i] = (char *)sp;
    }

    sp -= sizeof(char *);
    *(char **) sp = NULL;
    sp -= argv_size;
    memcpy((void *)sp, tmp_argv, argv_size);
    sp -= sizeof(size_t);
    *(size_t *) sp = argc;

    kfree_sync(tmp_argv);


    // actually copy over data after loading the arguments, or the
    // processes address space will be damaged, and we will get
    // incorrect arguments
    uintptr_t entry = load_elf_file(data);

    kfree_sync(data);

    //TODO make this more portable
    asm volatile ("mov %1, %%esp\n"
                  "jmp *%0\n"
                  :
                  :"r" (entry),
                   "r" (sp));
    // should be unreachable
    return 4;
}

ssize_t sys_exit(size_t code) {
    acquire_global();

    struct pcb *pcb = get_pcb(get_pid());
    pcb->return_code = code;

    // init does not have a parent
    if (pcb->ppid > 0) {
        struct pcb *ppcb = get_pcb(pcb->ppid);

        // wake waiting process if it exists
        if (ppcb->rs == RS_BLOCKED && (ppcb->wait_pid == pcb->pid ||
                                       ppcb->wait_pid == -1)) {

            ppcb->wait_pid = pcb->pid;
            ppcb->rs = RS_READY;
        }
    }

    for (size_t i = 0; i < MAX_FDS; ++i) {
        if (pcb->fds[i].file != NULL) {
            release_fd(pcb, i);
        }
    }

    pcb->rs = RS_TERMINATED;
    free_proc_addr_space(pcb->addr_space);

    pcb->addr_space = NULL;
    memset(pcb->fds, 0, sizeof(pcb->fds));

    release_global();

    sched();
    //should be unreachable
    return 4;
}
