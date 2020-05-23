#include "tty.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "arch/cpu.h"
#include "kdebug.h"

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

static void onkeypress(int scancode);

static size_t row, col;
static enum vga_color fg;
static enum vga_color bg;

void tty_init(void) {
    term_clear();
    row = col = 0;
    term_curto(row, col);
    fg = VGA_COLOR_LIGHT_GREY;
    bg = VGA_COLOR_BLACK;
    register_keypress(onkeypress);
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

// output functions
static void term_putchar(char c) {
    if (c == '\n') {
        ++row;
        col = 0;
    } else if (c == '\r') {
        col = 0;
    } else if (c == '\t') {
        col += 8;
    } else if (c == '\b') {
        if (--col == -1) {
            col = VGA_WIDTH;
            --row;
        }
        struct vga_entry *entry = &(term_buff[row*VGA_WIDTH+col]);
        entry->ch = ' ';
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

#define BUFF_LEN 2048

static char filebuff[BUFF_LEN]; // probably make this grow
static size_t fbase = 0;
static size_t lbase  = 0;
static size_t loff   = 0;

static bool shifted = false;

static char sc_to_ascii(int scancode) {
    if (shifted) {
        return en_shift_map[scancode];
    } else {
        return en_map[scancode];
    }
}


// interrupt handler
static void onkeypress(int scancode) {
    if (scancode == 0x36) {
        shifted = true;
    } else if (scancode == 0xb6) {
        shifted = false;
    } else if (scancode < 0x80) {
        char ch = sc_to_ascii(scancode);
        if (ch != 0) {
            if (ch == '\b') {
                if (loff != lbase) {
                    loff = (((loff - 1)%BUFF_LEN)+BUFF_LEN) % BUFF_LEN;
                    tty_write("\b", 1);
                }
            } else {

                filebuff[loff] = ch;
                loff = (loff+1) % BUFF_LEN;
                tty_write(&ch, 1);

                if (ch == '\n') {
                    lbase = loff;
                }
            }
        }
    }
}

#define MIN(a, b) ((a < b)? a:b)

ssize_t tty_read(void *buf, size_t count) {
    ssize_t c = count;
    while (count > 0) {
        cli();
        if (lbase > fbase) {
            size_t start = fbase;
            size_t len = MIN(lbase-fbase, count);
            fbase += len;
            sti();

            memcpy(buf, filebuff+start, len);
            count -= len;
            buf += len;
        } else if (lbase < fbase) {
            size_t start1 = fbase;
            size_t len1 = MIN(BUFF_LEN - fbase, count);
            fbase += len1;
            fbase %= BUFF_LEN;
            count -= len1;
            size_t start2 = fbase;
            size_t len2 = MIN(lbase - 0, count);
            fbase += len1;
            fbase %= BUFF_LEN;
            count -= len1;
            sti();

            memcpy(buf, filebuff+start1, len1);
            memcpy(buf+len1, filebuff+start2, len2);
            buf += len1+len2;
        }
        sti();
        //TODO replace with a scheduled lock
        if (count > 0) {
            halt();
        }
    }
    return c;
}
