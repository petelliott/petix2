#include "fb.h"
#include "../device.h"
#include "../proc.h"
#include "../mem.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

static bool fb_initialized = false;
static size_t width, height;
static fb_pixel_t *fb_addr;


static int fb_open(struct inode *in, struct file *file, int flags) {
    if (!fb_initialized) {
        return -ENXIO;
    }
    file->size = width * height * sizeof(fb_pixel_t);
    return 0;
}

static int fb_ioctl(struct file *f, size_t req, va_list ap) {
    if (req == FB_IOCTL_SIZE) {
        size_t *sp = va_arg(ap, size_t *);
        *sp = width * height * sizeof(fb_pixel_t);
        return 0;
    } else if (req == FB_IOCTL_GET_RESOLUTION) {
        int *w = va_arg(ap, int *);
        int *h = va_arg(ap, int *);
        *w = width;
        *h = height;
        return 0;
    } else {
        return -EINVAL;
    }
}

static void *fb_mmap(struct file *f, void *addr, size_t len, int prot,
                     int flags, off_t off, int *errno) {
    if (flags & (MAP_FIXED | MAP_PRIVATE)) {
        *errno = ENOTSUP;
        return MAP_FAILED;
    }

    if (len != (width * height * sizeof(fb_pixel_t))) {
        *errno = EINVAL;
        return MAP_FAILED;
    }

    struct pcb *pcb = get_pcb(get_pid());

    char *cfb_addr = (char *)fb_addr;
    char *caddr = addr;
    for (size_t i = 0; i < len; i += PAGE_SIZE) {
        if (remap_page_user(pcb->addr_space, caddr+i, cfb_addr+i) == -1) {
            *errno = EFAULT;
            return MAP_FAILED;
        }
    }

    flush_tlb();

    return addr;
}

void fb_init(fb_pixel_t *a, size_t w, size_t h) {
    fb_addr = a;
    width = w;
    height = h;
    fb_initialized = true;

    static struct file_ops fops = {
        .open = fb_open,
        .ioctl = fb_ioctl,
        .mmap = fb_mmap,
    };

    register_device(DEV_FB, &fops);
}
