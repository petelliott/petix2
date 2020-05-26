#include "proc.h"
#include "kdebug.h"
#include "sync.h"

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
    acquire_global();

    size_t init_off = pt_free;

    while (ptable[pt_free].rs != RS_NOPROC) {
        pt_free = (pt_free + 1) % PTABLE_SIZE;
        // out of pids
        kassert(pt_free != init_off);
    }

    struct pcb *pcb = &(ptable[pt_free]);

    pcb->rs = RS_CREATED;
    pcb->pid = make_pid(pid_gen(pcb->pid) + 1, pt_free);

    release_global();

    return pcb;
}


void sched(void) {
    panic("TODO: write scheduler");
}

// fork process proc into another process
struct pcb *proc_fork(struct pcb *proc) {
    struct pcb *newproc = alloc_proc();
    newproc->ppid = proc->pid;
    newproc->addr_space = fork_proc_addr_space(proc->addr_space);
    newproc->stack_ptr = proc->stack_ptr;
    return newproc;
}
