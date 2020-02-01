// raw vga printing, for before our tty is up
#include "debug.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



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

void term_curto(size_t r, size_t c) {
    uint16_t pos = r * VGA_WIDTH + c;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static struct vga_entry *term_buff = (struct vga_entry *) 0xB8000;

static size_t row, col;


void term_clear(void) {
    row = 0;
    col = 0;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			term_buff[index].ch = ' ';
			term_buff[index].bg = VGA_COLOR_BLACK;
		}
	}
}

void scroll_up(void) {
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


void term_putchar(char c, enum vga_color fg, enum vga_color bg) {
    if (c == '\n') {
        ++row;
        col = 0;
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
    //TODO scroll
    if (row == VGA_HEIGHT) {
        scroll_up();
    }
}

void term_writes(const char *str, enum vga_color fg, enum vga_color bg) {
    for (size_t i = 0; str[i]; ++i) {
        term_putchar(str[i], fg, bg);
    }
}

void print(const char *str) {
	term_writes(str, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    term_curto(row, col);
}

void warn(const char *str) {
	term_writes(str, VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    term_curto(row, col);
}

void error(const char *str) {
	term_writes(str, VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
    term_curto(row, col);
}
