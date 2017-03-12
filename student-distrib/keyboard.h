#ifndef KEYB_H
#define KEYB_H

#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ 1

//	PC/2 Keyboard ports
#define STATUS_PORT 0x64
#define DATA_PORT	0x60
#define	INBUF_MASK	0x02	//input buffer(bit 1) of status register: 1 = full, 0 = empty

extern void keyboard_init();
extern void keyboard_interrupt();

#endif
