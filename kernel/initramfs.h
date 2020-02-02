#ifndef INITRAMFS_H
#define INITRAMFS_H

#include <sys/types.h>
#include "tar.h"

void initramfs_init(struct tar *tar);

int initramfs_open(const char *pathname);
int initramfs_close(int fd);

ssize_t initramfs_read(int fd, void *buf, size_t count);

#endif
