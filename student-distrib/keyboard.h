#ifndef KEYB_H
#define KEYB_H

#include "driver.h"
#include "i8259.h"
#include "multi_term.h"

// For testing RTC
#include "rtc.h"
// For testing filesystem
#include "file_system.h"

#define KEYBOARD_IRQ    1
#define KEY_MAP_SIZE    128

// PS/2 ports
#define STATUS_PORT 0x64
#define DATA_PORT   0x60
// Input buffer (bit 1) of status register: 1 = full, 0 = empty
#define INBUF_MASK  0x02

extern void keyboard_init();
extern void keyboard_interrupt();

void process_input(char c);
uint8_t get_keymap(char c);

void clear_cur_cmd();
void add_to_history(int8_t *command);
void clear_buffer(void);

extern unsigned char read_buf[KEY_BUF_SIZE+1];
extern uint32_t read_buf_index;
extern bool new_line;

#endif
