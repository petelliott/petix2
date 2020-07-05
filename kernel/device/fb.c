#include "fb.h"
#include "../device.h"
#include "../proc.h"
#include "../mem.h"
#include "../kdebug.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

static bool fb_initialized = false;
size_t fb_width, fb_height;
static fb_pixel_t *fb_addr;

//TODO: non-dynamically sized buffer
//TODO: don't map over real usedful memory
fb_pixel_t kernel_framebuffer[786432] __attribute__((aligned(4096)));

static int fb_open(struct inode *in, struct file *file, int flags) {
    if (!fb_initialized) {
        return -ENXIO;
    }
    file->size = fb_width * fb_height * sizeof(fb_pixel_t);
    return 0;
}

static int fb_ioctl(struct file *f, size_t req, va_list ap) {
    if (req == FB_IOCTL_SIZE) {
        size_t *sp = va_arg(ap, size_t *);
        *sp = fb_width * fb_height * sizeof(fb_pixel_t);
        return 0;
    } else if (req == FB_IOCTL_GET_RESOLUTION) {
        int *w = va_arg(ap, int *);
        int *h = va_arg(ap, int *);
        *w = fb_width;
        *h = fb_height;
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

    if (len != (fb_width * fb_height * sizeof(fb_pixel_t))) {
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
    fb_width = w;
    fb_height = h;
    fb_initialized = true;

    static struct file_ops fops = {
        .open = fb_open,
        .ioctl = fb_ioctl,
        .mmap = fb_mmap,
    };

    kassert(sizeof(kernel_framebuffer) == (fb_width * fb_height * sizeof(fb_pixel_t)));

    char *cfb_addr = (char *)fb_addr;
    char *caddr = (char *) kernel_framebuffer;
    for (size_t i = 0; i < sizeof(kernel_framebuffer); i += PAGE_SIZE) {
        remap_page_kernel(caddr+i, cfb_addr+i);
    }

    register_device(DEV_FB, &fops);
}
