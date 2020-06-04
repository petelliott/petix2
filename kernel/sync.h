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

struct sem_proc_lst {
    pid_t pid;
    size_t needed;
    struct sem_proc_lst *next;
};

typedef struct {
    size_t count;
    struct sem_proc_lst *lst;
} petix_sem_t;

// n=0 => binary semaphore
void sem_signal(petix_sem_t *sem, size_t n);
void sem_wait(petix_sem_t *sem, size_t n);

// condition variables are binary semaphores
void cond_wake(petix_sem_t *sem);
void cond_wait(petix_sem_t *sem);


#endif
