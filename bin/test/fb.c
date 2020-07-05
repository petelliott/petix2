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

    int w, h;
    if (ioctl(fd, FB_IOCTL_GET_RESOLUTION, &w, &h) == -1) {
        perror("ioctl(2)");
        return 1;
    }

    printf("resolution=%ix%i\n", w, h);

    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            int color = (x/8 + y/16) % 3;
            fb[y*w+x] = 0xff0000 >> (color * 8);
        }
    }

    close(fd);
    return 0;
}
