#ifndef proc_h
#define proc_h

#include "arch/paging.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef size_t pid_t;

enum ready_state {
    RS_CREATED,
    RS_READY,
    RS_RUNNING,
    RS_BLOCKED,
    RS_TERMINATED,
    RS_NOPROC = 0
};

struct pcb {
    addr_space_t addr_space;
    pid_t pid;
    pid_t ppid;
    uintptr_t stack_ptr;
    enum ready_state rs;
    //TODO all kinds of other stuff
};

// sets up an empty process, ready for exec
void init_proc(void);

pid_t get_pid(void);
struct pcb *get_pcb(pid_t pid);

struct pcb *alloc_proc(void);


void sched(void);

// fork process proc into another process
struct pcb *proc_fork(struct pcb *proc);

#endif
