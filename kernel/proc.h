#ifndef proc_h
#define proc_h

#include "arch/paging.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "fs.h"


enum ready_state {
    RS_CREATED,
    RS_READY,
    RS_RUNNING,
    RS_BLOCKED,
    RS_TERMINATED,
    RS_NOPROC = 0
};

#define MAX_FDS 16

struct pcb {
    addr_space_t addr_space;
    pid_t pid;
    pid_t ppid;
    uintptr_t stack_ptr;
    enum ready_state rs;
    uint8_t return_code;

    struct {
        struct file file;
        bool valid;
    } fds[MAX_FDS];

    //TODO all kinds of other stuff
};

// sets up an empty process, ready for exec
void init_proc(void);

pid_t get_pid(void);
struct pcb *get_pcb(pid_t pid);

struct pcb *alloc_proc(void);

int alloc_fd(struct pcb *pcb);
void release_fd(struct pcb *pcb, int i);

void sched(void);


#endif
