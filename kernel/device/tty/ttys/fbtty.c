#include "fbtty.h"
#include "../../fb.h"
#include "../../../device.h"
#include "../../../kdebug.h"
#include "../../../kmalloc.h"
#include "../ansiseq.h"
#include "../ttylib.h"
#include "../kbd.h"
#include <errno.h>
#include <string.h>

#define CH_WIDTH 8
#define CH_HEIGHT 16

#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

struct glyph {
    uint8_t rows[CH_HEIGHT];
};

static struct glyph glyphs[256];

static void load_psf_file(const char *path) {
    struct inode in;
    struct file f;
    kassert(fs_lookup(path, &in) >= 0);
    kassert(fs_open(&in, &f, 0) >= 0);

    kassert(f.fops->read != NULL);

    void *data = kmalloc_sync(f.size);
    kassert(f.fops->read(&f, data, f.size) >= 0);
    if (f.fops->close != NULL) {
        f.fops->close(&f);
    }

    PSF_font *header = data;
    kassert(header->magic == PSF_FONT_MAGIC);
    kassert(header->height == CH_HEIGHT);
    kassert(header->width == CH_WIDTH);

    struct glyph *table = (void *) ((char *)data + header->headersize);
    for (size_t i = 0; i < 256; ++i) {
        glyphs[i] = table[i];
    }

    kfree_sync(data);
}


static fb_pixel_t ansi_to_rgb[] = {
    [ANSI_BLACK]   = 0x000000,
    [ANSI_RED]     = 0xaa0000,
    [ANSI_GREEN]   = 0x00aa00,
    [ANSI_YELLOW]  = 0xaa5500,
    [ANSI_BLUE]    = 0x0000aa,
    [ANSI_MAGENTA] = 0xaa00aa,
    [ANSI_CYAN]    = 0x00aaaa,
    [ANSI_WHITE]   = 0xaaaaaa,
};

static fb_pixel_t ansi_to_rgb_bold[] = {
    [ANSI_BLACK]   = 0x555555,
    [ANSI_RED]     = 0xff5555,
    [ANSI_GREEN]   = 0x55ff55,
    [ANSI_YELLOW]  = 0xffff55,
    [ANSI_BLUE]    = 0x5555ff,
    [ANSI_MAGENTA] = 0xff55ff,
    [ANSI_CYAN]    = 0x55ffff,
    [ANSI_WHITE]   = 0xffffff,
};

static void f_putch(const struct ansi_rendition *r, char ch, int row, int col) {
    fb_pixel_t fg_colour;
    fb_pixel_t bg_colour;

    if (r->fg == ANSI_DEFAULT) {
        if (r->flags.f.bold) {
            fg_colour = ansi_to_rgb_bold[ANSI_WHITE];
        } else {
            fg_colour = ansi_to_rgb[ANSI_WHITE];
        }
    } else {
        if (r->flags.f.bold) {
            fg_colour = ansi_to_rgb_bold[r->fg];
        } else {
            fg_colour = ansi_to_rgb[r->fg];
        }
    }

    if (r->bg == ANSI_DEFAULT) {
        bg_colour = ansi_to_rgb[ANSI_BLACK];
    } else {
        bg_colour = ansi_to_rgb[r->bg];
    }

    //TODO figure out blinking
    //entry->blink = r->flags.f.blink;

    if (r->flags.f.negative) {
        int tmp = fg_colour;
        fg_colour = bg_colour;
        bg_colour = tmp;
    }

    struct glyph *glyph = &(glyphs[(int)ch]);
    int ybase = row*CH_HEIGHT;
    int xbase = col*CH_WIDTH;
    for (int y = 0; y < CH_HEIGHT; ++y) {
        for (int x = 0; x < CH_WIDTH; ++x) {
            kernel_framebuffer[(ybase+y)*fb_width + (xbase+x)] =
                (glyph->rows[y] & (0x80 >> x))? fg_colour : bg_colour;
        }
    }
}

static int curx = -1, cury = -1;

static void f_curto(int row, int col) {
    if (curx != -1) {
        int ybase = cury*CH_HEIGHT;
        int xbase = curx*CH_WIDTH;

        for (int y = CH_HEIGHT-1; y < CH_HEIGHT; ++y) {
            for (int x = 0; x < CH_WIDTH; ++x) {
                    kernel_framebuffer[(ybase+y)*fb_width + (xbase+x)] =
                        ansi_to_rgb[ANSI_BLACK];
            }
        }
    }

    cury = row;
    curx = col;

    int ybase = cury*CH_HEIGHT;
    int xbase = curx*CH_WIDTH;

    for (int y = CH_HEIGHT-1; y < CH_HEIGHT; ++y) {
        for (int x = 0; x < CH_WIDTH; ++x) {
                kernel_framebuffer[(ybase+y)*fb_width + (xbase+x)] =
                    ansi_to_rgb[ANSI_WHITE];
        }
    }
}

static void curoff(void) {
    if (curx != -1) {
        int ybase = cury*CH_HEIGHT;
        int xbase = curx*CH_WIDTH;

        for (int y = CH_HEIGHT-1; y < CH_HEIGHT; ++y) {
            for (int x = 0; x < CH_WIDTH; ++x) {
                    kernel_framebuffer[(ybase+y)*fb_width + (xbase+x)] =
                        ansi_to_rgb[ANSI_BLACK];
            }
        }
    }
    curx = -1;
    cury = -1;
}

static struct ansi_backend ansi_backend;

static void f_scroll_up(void) {
    curoff();
    int pixels = fb_width * CH_HEIGHT;
	for (size_t y = 0; y < ansi_backend.row_n - 1; y++) {
        memcpy(&kernel_framebuffer[y*pixels],
               &kernel_framebuffer[(y+1)*pixels],
               pixels * sizeof(fb_pixel_t));
    }

    memset(&kernel_framebuffer[(ansi_backend.row_n-1)*pixels],
           0, pixels * sizeof(fb_pixel_t));
}

static struct ansi_backend ansi_backend = {
    .putch = f_putch,
    .curto = f_curto,
    .scroll_up = f_scroll_up,
    .row_n = 0,
    .col_n = 0,
};

static struct petix_tty tty;

static struct ansi_term ansi_term = {
    .backend = &ansi_backend,
    .tty     = &tty
};

static struct tty_backend tty_backend = {
    .row_n = 0,
    .col_n = 0,
    .backend_data = &ansi_term,
    .putch = (void (*)(void *, char))ansi_putch
};


static int dev_open(struct inode *in, struct file *file, int flags) {
    return 0;
}

static bool initialized = false;

static ssize_t dev_write(struct file *f, const char *buf, size_t count) {
    if (!initialized) {
        return -ENXIO;
    }
    return fbtty_write(buf, count);
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


void fbtty_init(void) {

    load_psf_file("/share/fonts/default8x16.psfu");

    ansi_backend.col_n = fb_width / CH_WIDTH;
    ansi_backend.row_n = fb_height / CH_HEIGHT;
    tty_backend.col_n = fb_width / CH_WIDTH;
    tty_backend.row_n = fb_height / CH_HEIGHT;

    ansi_init(&ansi_term);
    petix_tty_init(&tty, &tty_backend);

    register_kbd_tty(&tty);
    register_device(DEV_FBTTY, &fops);
    initialized = true;
}

ssize_t fbtty_write(const void *buf, size_t count) {
    if (!initialized) {
        return -ENXIO;
    }
    return petix_tty_write(&tty, buf, count);
}
