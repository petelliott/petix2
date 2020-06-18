#include "tty.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "../arch/cpu.h"
#include "../kdebug.h"
#include "../sync.h"
#include "../device.h"
#include "tty/ansiseq.h"
#include "tty/ttylib.h"

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

struct vga_entry {
    uint16_t ch    : 8;
    uint16_t fg    : 3;
    uint16_t bold  : 1;
    uint16_t bg    : 3;
    uint16_t blink : 1;
};


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static inline void outb(unsigned int port, unsigned char value) {
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

static void enable_blink(void) {

    //reset r/w flip-flop
    inb(0x3DA);

    outb(0x3C0, 0x30);
    uint8_t attr = inb(0x3C1);
    attr |= (1<<3);
    outb(0x3C0, attr);
}


static struct vga_entry *term_buff = (struct vga_entry *) 0xB8000;

static enum vga_color ansi_to_vga[] = {
    [ANSI_BLACK]   = VGA_COLOR_BLACK,
    [ANSI_RED]     = VGA_COLOR_RED,
    [ANSI_GREEN]   = VGA_COLOR_GREEN,
    [ANSI_YELLOW]  = VGA_COLOR_BROWN,
    [ANSI_BLUE]    = VGA_COLOR_BLUE,
    [ANSI_MAGENTA] = VGA_COLOR_MAGENTA,
    [ANSI_CYAN]    = VGA_COLOR_CYAN,
    [ANSI_WHITE]   = VGA_COLOR_LIGHT_GREY,
};

static void t_putch(const struct ansi_rendition *r, char ch, int row, int col) {
    struct vga_entry *entry = &(term_buff[row*VGA_WIDTH+col]);
    entry->ch = ch;

    if (r->fg == ANSI_DEFAULT) {
        entry->fg = VGA_COLOR_LIGHT_GREY;
    } else {
        entry->fg = ansi_to_vga[r->fg];
    }

    if (r->bg == ANSI_DEFAULT) {
        entry->bg = VGA_COLOR_BLACK;
    } else {
        entry->bg = ansi_to_vga[r->bg];
    }

    entry->bold = r->flags.f.bold;

    //TODO figure out blinking
    //entry->blink = r->flags.f.blink;

    if (r->flags.f.negative) {
        int tmp = entry->bg;
        entry->bg = entry->fg;
        entry->fg = tmp;
    }
}

static void t_curto(int row, int col) {
    uint16_t pos = row * VGA_WIDTH + col;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void t_scroll_up(void) {
	for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
            term_buff[y*VGA_WIDTH + x] = term_buff[(y+1)*VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT-1) * VGA_WIDTH + x;
        term_buff[index].ch = ' ';
        term_buff[index].bg = VGA_COLOR_BLACK;
        term_buff[index].fg = VGA_COLOR_LIGHT_GREY;
    }
}

static struct ansi_backend backend = {
    .putch = t_putch,
    .curto = t_curto,
    .scroll_up = t_scroll_up,
    .row_n = VGA_HEIGHT,
    .col_n = VGA_WIDTH
};

static struct petix_tty tty;

// device stuff


static int dev_open(struct inode *in, struct file *file, int flags) {
    file->private_data = false;
    return 0;
}

static ssize_t dev_write(struct file *f, const char *buf, size_t count) {
    return tty_write(buf, count);
}

static ssize_t dev_read(struct file *f, char *buf, size_t count) {
    return petix_tty_read(&tty, buf, count);
}

static int dev_ioctl(struct file *f, unsigned long req, va_list ap) {
    return petix_tty_ioctl(&tty, req, ap);
}

static struct file_ops fops = {
    .open  = dev_open,
    .write = dev_write,
    .read  = dev_read,
    .ioctl = dev_ioctl,
};

static void onkeypress(int scancode);

void tty_init(void) {
    enable_blink();
    petix_tty_init(&tty, &backend);

    register_device(DEV_TTY, &fops);
    register_keypress(onkeypress);
}

ssize_t tty_write(const void *buf, size_t count) {
    return petix_tty_write(&tty, buf, count);
}

// input functions


// stolen from serenityos
static const char en_map[0x80] = {
    0, '\033', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
    '=', 0x08, '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',',
    '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0,
};

static const char en_shift_map[0x80] = {
    0, '\033', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
    '+', 0x08, '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    '{', '}', '\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
    ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<',
    '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, '|', 0, 0, 0,
};


static bool shifted = false;
static bool ctrld   = false;

// returns -1 if invalid
static char sc_to_ascii(int scancode) {
    if (ctrld) {
        char ch = en_map[scancode];
        if (ch == 0) {
            return -1;
        }

        if (ch >= 'a') {
            return ch - 0x60;
        } else {
            return 0;
        }
    }

    char ret;
    if (shifted) {
        ret = en_shift_map[scancode];
    } else {
        ret = en_map[scancode];
    }

    if (ret == 0) {
        return -1;
    }
    return ret;
}

// interrupt handler
static void onkeypress(int scancode) {
    acquire_global();
    if (scancode == 0x36 || scancode == 0x2a) {
        shifted = true;
    } else if (scancode == 0xb6 || scancode == 0xaa) {
        shifted = false;
    } else if (scancode == 0x1d) {
        ctrld = true;
    } else if (scancode == 0x9d) {
        ctrld = false;
    } else if (scancode < 0x80) {
        char ch = sc_to_ascii(scancode);
        if (ch != -1) {
            petix_tty_input_seq(&tty, &ch, 1);
        }
    }
    release_global();
}
