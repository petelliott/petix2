#include "comtty.h"

#include "../ttylib.h"
#include "../../../arch/i686/io.h"
#include "../../../device.h"

#define COM1_DATA 0x3f8
#define COM1_IE (COM1_DATA + 1)


static void com_putch(void *backend_data, char ch) {
    outb(COM1_DATA, ch);
}

static struct tty_backend tty_backend = {
    .row_n = -1,
    .col_n = -1,
    .backend_data = NULL,
    .putch = com_putch,
};

static struct petix_tty tty;

static int dev_open(struct inode *in, struct file *file, int flags) {
    return 0;
}

static ssize_t dev_write(struct file *f, const char *buf, size_t count) {
    return comtty_write(buf, count);
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

void comtty_init(void) {
    petix_tty_init(&tty, &tty_backend);
    register_device(DEV_COMTTY, &fops);
}

ssize_t comtty_write(const void *buf, size_t count) {
    return petix_tty_write(&tty, buf, count);
}
