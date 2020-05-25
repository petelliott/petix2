#include "../../syscall.h"
#include "interrupts.h"
#include "../../kdebug.h"

void syscall_interrupt_handler(struct pushed_regs *regs) {
    kassert(regs->eax < 256);
    size_t ret = syscall_table[regs->eax](
                     regs->ebx,
                     regs->ecx,
                     regs->edx,
                     regs->esi,
                     regs->edi);

    regs->eax = ret;
}
