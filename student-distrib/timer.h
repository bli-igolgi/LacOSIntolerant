#ifndef TIMER_H
#define TIMER_H

#include "lib.h"
#include "i8259.h"
#include "multi_term.h"
#include "paging.h"
#include "syscalls.h"

#define PIT_IRQ         0
#define TIME_QUANTUM    40
#define PIT_FREQ        (1000/TIME_QUANTUM)       // The length each task will get, in milliseconds
#define BASE_FREQ       1193180
#define CHANNEL1_PORT   0x40
#define COMMAND_PORT    0x43

void pit_init(void);
void pit_interrupt(void);
void set_pit_freq(uint32_t hz);
void switch_tasks(int new_term_id);

#endif
