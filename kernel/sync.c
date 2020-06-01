#include "sync.h"
#include "proc.h"
#include "arch/cpu.h"
#include "kdebug.h"
#include "kmalloc.h"

static size_t acq_depth = 0;

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
