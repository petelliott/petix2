#ifndef PIPE_H
#define PIPE_H

#include "sync.h"
#include "fs.h"

#define PIPE_SIZE 4096

struct pipe {
    petix_lock_t lock;
    petix_sem_t rcond;
    petix_sem_t wcond;
    bool owrite;
    bool oread;
    size_t lo;
    size_t hi;
    char buffer[PIPE_SIZE];
};

void open_pipe(struct file *rfile, struct file *wfile);

#endif
