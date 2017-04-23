#ifndef MULT_TERM_H
#define MULT_TERM_H

#include "lib.h"
#include "paging.h"
#include "i8259.h"
#include "pcb.h"

// #define MAX_TERM_NUM    MAX_TERM_NUM
// Size of the input data, not including the null character
#define BUF_SIZE        128


typedef struct term_t {
    uint8_t term_id;                        // Which terminal this is
    uint8_t curs_x, curs_y, key_x, key_y;   // The cursor and keyboard positions
    uint8_t *vid_mem;                       // Pointer to this terminal's video memory
    uint8_t key_buf[BUF_SIZE+1];            // Buffer for the input data from the keyboard
    uint32_t key_buf_index;
    uint32_t esp, ebp;                      // The current terminals esp and ebp
    struct pcb_t *cur_task;                 // The task that is currently executing on the terminal
} term_t;

extern term_t terminals[3];
extern int cur_term_id;

void multi_term_init(void);
void switch_terminal(int new_term_id);
void switch_keyboard_and_cursor_pos(int new_term_id);
void switch_screen(int new_term_id);
void switch_stackframe(int new_term_id);

#endif
