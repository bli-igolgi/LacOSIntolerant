#ifndef MULT_TERM_H
#define MULT_TERM_H

#include "lib.h"
#include "paging.h"

#define MAX_TERM_NUM    3
// Size of the input data, not including the null character
#define BUF_SIZE        128


typedef struct term_t {
    uint8_t term_id;                        // Which terminal this is
    uint8_t curs_x, curs_y, key_x, key_y;   // The cursor and keyboard positions
    uint8_t *vid_mem;                       // Pointer to this terminal's video memory
    uint8_t key_buf[BUF_SIZE+1];      // Buffer for the input data from the keyboard
    uint32_t key_buf_index;
} term_t;

extern term_t terminals[3];
extern int cur_term_id;

void multi_term_init();
void switch_terminal(int new_term_id);
void switch_screen(int new_term_id);

#endif
