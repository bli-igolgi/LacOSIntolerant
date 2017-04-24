#ifndef TIMER_H
#define TIMER_H

#include "lib.h"
#include "i8259.h"
#include "multi_term.h"

#define PIT_IRQ         0
#define CHANNEL1_PORT   0x40
#define COMMAND_PORT    0x43

void pit_init(void);
void pit_interrupt(void);
void set_pit_freq(uint32_t hz);

#endif
