#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

#define WIDTH 512
#define HEIGHT 256

#define RED_COLOUR 0xe53935
#define BLUE_COLOUR 0x1e88e5


uint32_t fb[768][1024] __attribute__((aligned(4096)));

static struct termios save;
static int tlx,tly;

static void enable_raw(void) {
    // we use stdout as the terminal, because stdin can be piped to
    if (tcgetattr(STDOUT_FILENO, &save) == -1) {
        perror("tcgetattr(3)");
    }

    struct termios raw = save;
    raw.c_lflag &= ~(ECHO | ICANON);

    if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &raw) == -1) {
        perror("tcsetattr(3)");
    }
}

static void disable_raw(void) {
    if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, &save) == -1) {
        perror("tcsetattr(3)");
    }
}

static void get_screen(int width, int height) {
    int chheight = (height + CHAR_HEIGHT - 1) / CHAR_HEIGHT;
    for (int i = 0; i < chheight; ++i) {
        printf("\n");
    }

    write(STDIN_FILENO, "\e[6n", 4);

    char buff[11] = {0};
    read(STDIN_FILENO, buff, sizeof(buff));

    char *rowc = buff + 2;

    *strchr(rowc, ';') = 0;
    //char *colc = strchr(rowc, ';');
    //*colc = 0;
    //++colc;
    //*strchr(colc, 'R') = 0;

    tlx = 0;
    tly = (atoi(rowc) - chheight) * CHAR_HEIGHT;
}

static void draw_rect(int xx, int yy, int w, int h, uint32_t colour) {
    for (size_t y = tly+yy; y < tly+yy+h; ++y) {
        for (size_t x = tlx+xx; x < tlx+xx+w; ++x) {
            fb[y][x] = colour;
        }
    }
}

static void draw_border(void) {
    draw_rect(0, 0, WIDTH/2, 4, RED_COLOUR);
    draw_rect(0, HEIGHT-4, WIDTH/2, 4, RED_COLOUR);
    draw_rect(0, 0, 4, HEIGHT, RED_COLOUR);

    draw_rect(WIDTH/2, 0, WIDTH/2, 4, BLUE_COLOUR);
    draw_rect(WIDTH/2, HEIGHT-4, WIDTH/2, 4, BLUE_COLOUR);
    draw_rect(WIDTH-4, 0, 4, HEIGHT, BLUE_COLOUR);
}

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

    enable_raw();

    get_screen(WIDTH, HEIGHT);

    draw_border();


    disable_raw();
    close(fd);
    return 0;
}
