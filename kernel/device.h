#ifndef DEVICE_H
#define DEVICE_H

#include "fs.h"

enum device_majors {
    DEV_INITRD = 0,
    DEV_VGATTY = 1,
    DEV_RANDOM = 2,
    DEV_COMTTY = 3,
    DEV_FB     = 4,
    DEV_FBTTY  = 5,
};

#endif
