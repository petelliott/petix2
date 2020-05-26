#include "proc.h"
#include "kdebug.h"

#define PTABLE_SIZE 1024

#define pid_gen(pid) ((pid) >> 10)
#define pid_off(pid) ((pid) & (PTABLE_SIZE - 1))
#define make_pid(gen, off) (((gen) << 10) | (off))

static struct pcb ptable[PTABLE_SIZE];
static size_t pt_free = 0;

static pid_t curpid;

// sets up an empty process, ready for exec
void init_proc(void) {
    struct pcb *pcb = alloc_proc();
    curpid = pcb->pid;
    pcb->ppid = pcb->pid;
    pcb->addr_space = NULL;
    pcb->rs = RS_RUNNING;
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
    size_t init_off = pt_free;

    while (ptable[pt_free].rs != RS_NOPROC) {
        pt_free = (pt_free + 1) % PTABLE_SIZE;
        // out of pids
        kassert(pt_free != init_off);
    }

    ptable[pt_free].rs = RS_CREATED;
    ptable[pt_free].pid = make_pid(pid_gen(ptable[pt_free].pid) + 1, pt_free);

    return &(ptable[pt_free]);
}


void sched(void);

// fork process proc into another process
struct pcb *proc_fork(struct pcb *proc);
