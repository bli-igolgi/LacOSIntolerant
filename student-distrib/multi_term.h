#ifndef MULT_TERM_H
#define MULT_TERM_H

#include "lib.h"
#include "paging.h"
#include "i8259.h"
#include "pcb.h"

void multi_term_init(void);
void switch_terminal(int new_term_id);
void switch_keyboard_and_cursor_pos(int new_term_id);
void switch_screen(int new_term_id);

#endif
