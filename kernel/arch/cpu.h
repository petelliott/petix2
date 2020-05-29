#ifndef CPU_H
#define CPU_H

#include <stddef.h>

/* sets up the cpu/interrupts
   disables interrupts */
void init_cpu(void);

/* disable interrupts */
void cli(void);
/* enable interrupts */
void sti(void);

void halt(void);

typedef void(*keypress_cb_t)(int scancode);
void register_keypress(keypress_cb_t callback);

typedef void(*timer_cb_t)(void);
void register_timer(timer_cb_t callback);
void set_cpu_interval(size_t usecs);

#endif
