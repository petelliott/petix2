#ifndef DEVICE_FB_H
#define DEVICE_FB_H

#include <stdint.h>
#include <stddef.h>

typedef uint32_t fb_pixel_t;

void fb_init(fb_pixel_t *addr, size_t width, size_t height);

#endif
