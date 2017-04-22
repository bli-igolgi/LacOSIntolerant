/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"

static char* video_mem = (char *)VIDEO_ADDR;

term_t terminals[3];


void multi_term_init() {
    terminals[0].term_id = 1;
    terminals[1].term_id = 2;
    terminals[2].term_id = 3;

    terminals[0].vid_mem = (uint8_t *)TERM1_VID;
    terminals[1].vid_mem = (uint8_t *)TERM2_VID;
    terminals[2].vid_mem = (uint8_t *)TERM3_VID;
}

void switch_screen(int cur_term, int new_term) {
    int32_t i;
    uint8_t *cur_vid = terminals[cur_term].vid_mem,
            *new_vid = terminals[new_term].vid_mem;
    for(i = 0; i < NUM_COLS * NUM_ROWS; i++) {
        *(cur_vid + (i << 1)) = *(video_mem + (i << 1));
        *(cur_vid + (i << 1) + 1) = *(video_mem + (i << 1) + 1);

        *(video_mem + (i << 1)) = *(new_vid + (i << 1));
        *(video_mem + (i << 1) + 1) = *(new_vid + (i << 1) + 1);



        // Save temporarily for swapping
        // uint8_t term1_char = *(uint8_t *)(term1_vid + (i << 1));
        // uint8_t term1_attrib = *(uint8_t *)(term1_vid + (i << 1) + 1);

        // // Put terminal 2 into terminal 1
        // *(uint8_t *)(term1_vid + (i << 1)) = *(uint8_t *)(term2_vid + (i << 1));
        // *(uint8_t *)(term1_vid + (i << 1) + 1) = *(uint8_t *)(term2_vid + (i << 1) + 1);

        // // Put terminal 1 into terminal 2
        // *(uint8_t *)(term2_vid + (i << 1)) = term1_char;
        // *(uint8_t *)(term2_vid + (i << 1) + 1) = term1_attrib;
    }
    // Swap the pointers to each terminals' video memory
    // temp = term1_vid;
    // term1_vid = term2_vid;
    // term2_vid = temp;
}
