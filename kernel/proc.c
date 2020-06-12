#include "proc.h"
#include "kdebug.h"
#include "sync.h"
#include "arch/cpu.h"
#include "arch/switch.h"
#include "kmalloc.h"
#include <errno.h>
#include <string.h>

#define PTABLE_SIZE 1024

#define pid_gen(pid) ((pid) >> 10)
#define pid_off(pid) ((pid) & (PTABLE_SIZE - 1))
#define make_pid(gen, off) (((gen) << 10) | (off))

static struct pcb ptable[PTABLE_SIZE];
static size_t pt_free = 0;

static pid_t curpid;

static void timer_handler(void) {
    sched();
}

// sets up an empty process, ready for exec
void init_proc(void) {
    struct pcb *pcb = alloc_proc();
    curpid = pcb->pid;
    pcb->ppid = -8; // IDK
    pcb->addr_space = NULL;
    pcb->rs = RS_RUNNING;
    pcb->addr_space = create_proc_addr_space();
    use_addr_space(pcb->addr_space);

    memset(pcb->fds, 0, sizeof(pcb->fds));

    acquire_global();
    register_timer(timer_handler);
    set_cpu_interval(100000);

    enable_sched_locks();
    release_global();
}

pid_t get_pid(void) {
    return curpid;
}

struct pcb *get_pcb(pid_t pid) {
    struct pcb *pcb = &(ptable[pid_off(pid)]);

    if (pcb->rs == RS_NOPROC || pcb->pid != pid) {
        return NULL;
    }

    return pcb;
}

struct pcb *alloc_proc(void) {
    acquire_global();

    size_t init_off = pt_free;

    while (ptable[pt_free].rs != RS_NOPROC) {
        pt_free = (pt_free + 1) % PTABLE_SIZE;

        if (pt_free == init_off) {
            // out of pids
            return NULL;
        }
    }

    struct pcb *pcb = &(ptable[pt_free]);

    pcb->rs = RS_CREATED;
    pcb->pid = make_pid(pid_gen(pcb->pid) + 1, pt_free);
    pcb->ppid = -1;
    pcb->wait_pid = NOT_WAITING;

    release_global();

    return pcb;
}

int alloc_fd(struct pcb *pcb) {
    for (int i = 0; i < MAX_FDS; ++i) {
        if (pcb->fds[i] == NULL) {
            pcb->fds[i] = kmalloc_sync(sizeof(struct file));
            kprintf("allocating file %p\n", pcb->fds[i]);
            pcb->fds[i]->refcnt = 1;
            return i;
        }
    }
    return -EMFILE;
}

void release_fd(struct pcb *pcb, int i) {
    kassert(pcb->fds[i] != NULL);
    kassert(pcb->fds[i]->refcnt != 0);

    pcb->fds[i]->refcnt--;

    if (pcb->fds[i]->refcnt == 0) {
        kprintf("freeing file %p\n", pcb->fds[i]);
        kfree_sync(pcb->fds[i]);
    }
    pcb->fds[i] = NULL;
}

void dup_fd(struct pcb *pcb, int fd) {
    pcb->fds[fd]->refcnt++;
}

int proc_get_terminated_child(struct pcb *pcb, pid_t pid) {
    acquire_global();
    bool child = false;
    for (size_t i = 0; i < PTABLE_SIZE; ++i) {
        struct pcb *cpcb = &(ptable[i]);
        if (cpcb->rs == RS_NOPROC) {
            continue;
        }

        if (cpcb->ppid == pcb->pid && (pid == -1 || pid == cpcb->pid)) {
            if (cpcb->rs == RS_TERMINATED) {
                pcb->wait_pid = cpcb->pid;
                release_global();
                return 0;
            } else {
                child = true;
            }
        }
    }
    release_global();
    if (child) {
        return -1;
    } else {
        return -ECHILD;
    }
}

void sched(void) {
    static bool nested = false;
    if (nested) {
        return;
    }

    acquire_global();

    struct pcb *curpcb = &(ptable[pid_off(curpid)]);
    if (curpcb->rs == RS_RUNNING) {
        curpcb->rs = RS_READY;
    }

    size_t ioff = (pid_off(curpid) + 1) % PTABLE_SIZE;
    size_t off = ioff;

    bool blocked_p = false;

    while (ptable[off].rs != RS_READY) {
        blocked_p = blocked_p || (ptable[off].rs == RS_BLOCKED);
        off = (off + 1) % PTABLE_SIZE;

        if (off == ioff) {
            // out of pids, sleep until an interrupt changes that
            if (blocked_p) {
                //kprintf("no processes; halting until interrupt\n");
                nested = true;
                release_global();
                halt();
                acquire_global();
                nested = false;
            } else {
                panic("no running procs. TODO: shutdown");
            }
        }
    }

    struct pcb *newpcb = &(ptable[off]);
    if (newpcb != curpcb) {
        // context switch does not work with the same process
        newpcb->rs = RS_RUNNING;
        curpid = newpcb->pid;

        //panic("debu");
        context_switch(newpcb->stack_ptr, &(curpcb->stack_ptr), newpcb->addr_space);
    } else {
        curpcb->rs = RS_RUNNING;
    }

    release_global();
}
