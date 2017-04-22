#ifndef MULT_TERM_H
#define MULT_TERM_H

#include "lib.h"
#include "paging.h"


typedef struct term_t {
    uint8_t term_id;                        // Which terminal this is
    uint8_t curs_x, curs_y, key_x, key_y;   // The cursor and keyboard positions
    uint8_t *vid_mem;                       // Pointer to this terminal's video memory
} term_t;

extern term_t terminals[3];
extern term_t *cur_term;

void multi_term_init();
void switch_screen(int cur_term, int new_term);

#endif
