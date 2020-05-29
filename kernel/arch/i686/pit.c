#include "../cpu.h"
#include "interrupts.h"
#include "../../sync.h"
#include "../../kdebug.h"
#include "io.h"

static const double pitfreq = 1.193181666666666; // MHz = cycles/usec

// io registers
static const uint16_t c0_data  = 0x40;
//static const uint16_t c1_data  = 0x41;
//static const uint16_t c2_data  = 0x42;
static const uint16_t mode_com = 0x43;

#define SELECT_0    (3 << 6)
#define AM_LOHI     (3 << 4)
#define MODE_SQUARE (3 << 1)


static timer_cb_t timer_callback = NULL;
static size_t softdiv;
static size_t sdc = 0;


static void timer_interrupt_handler(struct pushed_regs *regs) {
    send_eoi(regs->irq);

    if (softdiv == 0) {
        timer_callback();
    } else {
        if (sdc == 0) {
            timer_callback();
            sdc = softdiv;
        }
        --sdc;
    }
}

void register_timer(timer_cb_t callback) {
    register_interrupt_handler(32, timer_interrupt_handler);
    timer_callback = callback;
}

void set_cpu_interval(size_t usecs) {
    size_t fdiv = (size_t) (pitfreq * usecs);

    uint16_t reload;
    if (fdiv > 65536) {
        reload = 0;
        softdiv = fdiv / 65536;
    } else {
        reload = fdiv;
        softdiv = 0;
    }

    kassert(reload != 1);

    acquire_global();

    outb(mode_com, SELECT_0 | AM_LOHI | MODE_SQUARE);

    outb(c0_data, reload & 0xff);
    outb(c0_data, (reload >> 8) & 0xff);

    release_global();
}
