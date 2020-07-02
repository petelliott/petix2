#include "../switch.h"
#include "tables.h"

void set_hardware_kernel_stack(void *sp) {
    tss.esp0 = (uintptr_t) sp;
}
