/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"

static char* video_mem = (char *)VIDEO_ADDR;

term_t terminals[3];
term_t *cur_term;


void multi_term_init() {
    terminals[0].term_id = 1;
    terminals[1].term_id = 2;
    terminals[2].term_id = 3;

    terminals[0].vid_mem = (uint8_t *)TERM1_VID;
    terminals[1].vid_mem = (uint8_t *)TERM2_VID;
    terminals[2].vid_mem = (uint8_t *)TERM3_VID;

    cur_term = &terminals[0];
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
    }
}
