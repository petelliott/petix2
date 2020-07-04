#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>


uint32_t fb[786432] __attribute__((aligned(4096)));

int main(int argc, char *argv[]) {
    int fd = open("/dev/fb", 0);
    if (fd == -1) {
        perror("open(2)");
        return 1;
    }
    if (mmap(fb, sizeof(fb), 0, MAP_SHARED, fd, 0) == MAP_FAILED) {
        perror("mmap(2)");
        return 1;
    }

    for (size_t i = 0; i < 786432; ++i) {
        fb[i] = 0x32a852;
    }

    close(fd);
    return 0;
}
