#ifndef KEYB_H
#define KEYB_H

#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ 1

extern void keyboard_init();
extern void keyboard_interrupt(char c);

#endif
