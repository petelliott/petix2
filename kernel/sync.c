#include "sync.h"
#include "proc.h"
#include "arch/cpu.h"
#include "kdebug.h"
#include "kmalloc.h"

static ssize_t acq_depth = 0;

void acquire_global(void) {
    cli();
    acq_depth++;
}

void release_global(void) {
    cli();
    acq_depth--;

    kassert(acq_depth >= 0);

    if (acq_depth == 0) {
        sti();
    }
}

bool is_global_held(void) {
    return acq_depth != 0;
}

static bool slocks = false;

void enable_sched_locks(void) {
    slocks = true;
}

void acquire_lock(petix_lock_t *lock) {
    acquire_global();

    if (slocks) {
        if (lock->locked && (lock->held_by == get_pid() || lock->global)) {
            panic("attempt to re-acquire lock");
        } else if (lock->locked) {
            struct proc_lst *nplist = kmalloc(sizeof(struct proc_lst));
            nplist->pid = get_pid();
            nplist->next = lock->lst;
            lock->lst = nplist;

            struct pcb *pcb = get_pcb(get_pid());
            pcb->rs = RS_BLOCKED;

            release_global();
            sched();
            acquire_lock(lock);
            return;
        } else {
            lock->locked = true;
            lock->held_by = get_pid();
            lock->global = false;
        }
    } else {
        kassert(lock->locked == false);
        lock->global = true;
        lock->locked = true;
    }

    release_global();
}

void release_lock(petix_lock_t *lock) {
    acquire_global();

    if (lock->global) {
        kassert(lock->locked == true);
        lock->global = false;
        lock->locked = false;
    } else {
        if (!(lock->locked && (lock->held_by == get_pid() || lock->global))) {
            panic("attempt to release un-acquired lock");
        } else if (lock->locked) {
            lock->locked = false;
            lock->held_by = 0;
            for (struct proc_lst *lst = lock->lst; lst != NULL;
                 lst = lst->next, kfree(lst)) {
                struct pcb *pcb = get_pcb(lst->pid);
                pcb->rs = RS_READY;
            }
            lock->lst = NULL;
        }
    }

    release_global();
}

void sem_signal(petix_sem_t *sem, size_t n) {
    acquire_global();

    if (n == 0) {
        sem->count = 1;
    } else {
        sem->count += n;
    }
    struct sem_proc_lst *onto = NULL;
    for (struct sem_proc_lst *lst = sem->lst; lst != NULL;) {
        struct sem_proc_lst *next = lst->next;

        if (lst->needed <= sem->count) {
            struct pcb *pcb = get_pcb(lst->pid);
            kassert(pcb->rs == RS_BLOCKED);
            pcb->rs = RS_READY;

            kfree(lst);
        } else {
            // push back on the list
            lst->next = onto;
            onto = lst;
        }

        lst = next;
    }
    sem->lst = onto;

    release_global();
}

void sem_wait(petix_sem_t *sem, size_t n) {
    acquire_global();

    if (n == 0) {
        n = 1;
    }

    struct pcb *pcb = get_pcb(get_pid());
    while(1) {
        kassert(pcb->rs == RS_RUNNING);

        if (sem->count >= n) {
            sem->count -= n;
            break;
        }

        struct sem_proc_lst *nplist = kmalloc(sizeof(struct sem_proc_lst));
        nplist->pid = get_pid();
        nplist->needed = n;
        nplist->next = sem->lst;
        sem->lst = nplist;

        pcb->rs = RS_BLOCKED;
        release_global();
        sched();
        acquire_global();
    }

    release_global();
}

void cond_wake(petix_sem_t *sem) {
    sem_signal(sem, 0);
}

void cond_wait(petix_sem_t *sem) {
    sem_wait(sem, 0);
}
