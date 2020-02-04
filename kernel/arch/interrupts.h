#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void init_interrupts(void);

/* disable interrupts */
void cli(void);
/* enable interrupts */
void sti(void);

typedef void(*keypress_cb_t)(int scancode);

void register_keypress(keypress_cb_t callback);

#endif
