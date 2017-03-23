#ifndef KEYB_H
#define KEYB_H

#include "driver.h"
#include "i8259.h"

// For testing RTC
#include "rtc.h"

#define KEYBOARD_IRQ 1
#define BUF_SIZE     128
#define KEY_MAP_SIZE 128

// PC/2 Keyboard ports
#define STATUS_PORT 0x64
#define DATA_PORT	0x60
#define	INBUF_MASK	0x02	//input buffer(bit 1) of status register: 1 = full, 0 = empty

extern void keyboard_init();
extern void keyboard_interrupt();

void process_input(char c);
void clear_buffer(void);

extern unsigned char read_buf[BUF_SIZE];
extern bool new_line;

#endif
