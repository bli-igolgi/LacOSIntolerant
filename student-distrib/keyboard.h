#ifndef KEYB_H
#define KEYB_H

#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ 1

#define STATUS_PORT 0x64
#define DATA_PORT	0x60

extern void keyboard_init();
extern void keyboard_interrupt();
extern void free_keyboard();

#endif
