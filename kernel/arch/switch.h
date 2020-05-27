#ifndef ARCH_SWITCH_H
#define ARCH_SWITCH_H

#include <stdint.h>
#include "paging.h"

void context_switch(uintptr_t newesp, uintptr_t *saveesp, addr_space_t as);

void fork_switchable(uintptr_t *saveesp, addr_space_t ias, addr_space_t *as);

#endif
