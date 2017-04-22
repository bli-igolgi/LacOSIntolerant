#ifndef MULT_TERM_H
#define MULT_TERM_H

#include "lib.h"


typedef struct term_t {
    uint8_t term_id;                        // Which terminal this is
    uint8_t curs_x, curs_y, key_x, key_y;   // The cursor and keyboard positions
    uint8_t *vid_mem;                       // Pointer to this terminal's video memory
} term_t;


void switch_screen(term_t *term1, term_t *term2);

#endif
