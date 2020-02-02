#include "tty.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
    uint16_t ch : 8;
    uint16_t fg : 4;
    uint16_t bg : 4;
};


static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static inline void outb(unsigned int port, unsigned char value) {
   asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

static void term_curto(size_t r, size_t c) {
    uint16_t pos = r * VGA_WIDTH + c;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static struct vga_entry *term_buff = (struct vga_entry *) 0xB8000;

static void term_clear(void) {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			term_buff[index].ch = ' ';
			term_buff[index].bg = VGA_COLOR_BLACK;
		}
	}
}

static size_t row, col;
static enum vga_color fg;
static enum vga_color bg;

void tty_init(void) {
    term_clear();
    row = col = 0;
    term_curto(row, col);
    fg = VGA_COLOR_LIGHT_GREY;
    bg = VGA_COLOR_BLACK;
}

static void scroll_up(void) {
	for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
            term_buff[y*VGA_WIDTH + x] = term_buff[(y+1)*VGA_WIDTH + x];
        }
    }
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT-1) * VGA_WIDTH + x;
        term_buff[index].ch = ' ';
        term_buff[index].bg = VGA_COLOR_BLACK;
    }
    row--;
}

static void term_putchar(char c) {
    if (c == '\n') {
        ++row;
        col = 0;
    } else if (c == '\r') {
        col = 0;
    } else if (c == '\t') {
        col += 8;
    } else if (c == '\e') {
        //TODO: escape sequences
    } else {
        struct vga_entry *entry = &(term_buff[row*VGA_WIDTH+col]);
        entry->ch = c;
        entry->fg = fg;
        entry->bg = bg;
        if (++col == VGA_WIDTH) {
            col = 0;
            ++row;
        }
    }
    if (row == VGA_HEIGHT) {
        scroll_up();
    }
}

ssize_t tty_write(const void *buf, size_t count) {

    for (size_t i = 0; i < count; ++i) {
        term_putchar(((const char *)buf)[i]);
    }
    term_curto(row, col);
    return count;
}
