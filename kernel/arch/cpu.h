#ifndef CPU_H
#define CPU_H

/* sets up the cpu/interrupts
   disables interrupts */
void init_cpu(void);

/* disable interrupts */
void cli(void);
/* enable interrupts */
void sti(void);

typedef void(*keypress_cb_t)(int scancode);

void register_keypress(keypress_cb_t callback);

#endif
