#ifndef DEVICE_FB_H
#define DEVICE_FB_H

#include <stdint.h>
#include <stddef.h>

typedef uint32_t fb_pixel_t;

extern size_t fb_width, fb_height;
extern fb_pixel_t kernel_framebuffer[786432];

void fb_init(fb_pixel_t *addr, size_t width, size_t height);

#endif
