#ifndef SYNC_H
#define SYNC_H

#include <sys/types.h>
#include <stdbool.h>

void acquire_global(void);
void release_global(void);

bool is_global_held(void);

struct proc_lst {
    pid_t pid;
    struct proc_lst *next;
};

typedef struct {
    struct proc_lst *lst;
    pid_t held_by;
    bool locked;
    bool global;
} petix_lock_t;

void enable_sched_locks(void);

void acquire_lock(petix_lock_t *lock);
void release_lock(petix_lock_t *lock);

typedef struct {
    struct proc_lst *lst;
} petix_conition_t;


#endif
