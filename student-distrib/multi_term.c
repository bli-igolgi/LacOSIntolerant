/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"



void switch_screen(term_t *term1, term_t *term2) {
    int32_t i;
    uint8_t *term1_vid = term1->vid_mem,
            *term2_vid = term2->vid_mem,
            *temp;
    for(i = 0; i < NUM_COLS * NUM_ROWS; i++) {
        // Save temporarily for swapping
        uint8_t term1_char = *(uint8_t *)(term1_vid + (i << 1));
        uint8_t term1_attrib = *(uint8_t *)(term1_vid + (i << 1) + 1);

        // Put terminal 2 into terminal 1
        *(uint8_t *)(term1_vid + (i << 1)) = *(uint8_t *)(term2_vid + (i << 1));
        *(uint8_t *)(term1_vid + (i << 1) + 1) = *(uint8_t *)(term2_vid + (i << 1) + 1);

        // Put terminal 1 into terminal 2
        *(uint8_t *)(term2_vid + (i << 1)) = term1_char;
        *(uint8_t *)(term2_vid + (i << 1) + 1) = term1_attrib;
    }
    // Swap the pointers to each terminals' video memory
    temp = term1_vid;
    term1_vid = term2_vid;
    term2_vid = temp;
}
