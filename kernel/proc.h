#ifndef proc_h
#define proc_h

#include "arch/paging.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include "fs.h"


enum ready_state {
    RS_NOPROC = 0,
    RS_CREATED,
    RS_READY,
    RS_RUNNING,
    RS_BLOCKED,
    RS_TERMINATED,
};

#define MAX_FDS 16

#define NOT_WAITING -2

#define KERNEL_STACK_SIZE 32768
#define STACK_TOP (void*)0xffffffff

struct pcb {
    addr_space_t addr_space;
    pid_t pid;
    pid_t ppid;
    uintptr_t stack_ptr;
    enum ready_state rs;
    uint8_t return_code;
    pid_t wait_pid;
    struct {
        struct file *file;
        bool cloexec;
    } fds[MAX_FDS];

    //TODO all kinds of other stuff
};

// sets up an empty process, ready for exec
void init_proc(void);

pid_t get_pid(void);
struct pcb *get_pcb(pid_t pid);

struct pcb *alloc_proc(void);
void release_proc(struct pcb *);

int alloc_fd(struct pcb *pcb);
void release_fd(struct pcb *pcb, int fd);
void dup_fd(struct pcb *pcb, int fd);

// waits for a child and sets wait_pid if -1
int proc_get_terminated_child(struct pcb *pcb, pid_t pid);

void sched(void);


#endif
