#include "proc.h"
#include "kdebug.h"
#include "sync.h"
#include "arch/cpu.h"
#include "arch/switch.h"

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
    pcb->ppid = pcb->pid;
    pcb->addr_space = NULL;
    pcb->rs = RS_RUNNING;


    register_timer(timer_handler);
    set_cpu_interval(100000);
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
                kprintf("no processes; halting until interrupt\n");
                release_global();
                halt();
                acquire_global();
            } else {
                panic("no running procs. TODO: shutdown");
            }
        }
    }

    struct pcb *newpcb = &(ptable[off]);
    newpcb->rs = RS_RUNNING;
    curpid = newpcb->pid;

    //panic("debu");
    context_switch(newpcb->stack_ptr, &(curpcb->stack_ptr), newpcb->addr_space);

    release_global();
}
