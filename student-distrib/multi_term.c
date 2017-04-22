/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"

static char* video_mem = (char *)VIDEO_ADDR;

term_t terminals[3];

void multi_term_init() {
    uint32_t i, j=TERM1_VID;
    for(i=1;i<=3;++i){
        terminals[i-1].term_id = i;
        terminals[i-1].vid_mem = (uint8_t *)(j+i*0x1000);
    }
}

void switch_screen(int cur_term, int new_term) {
    int32_t i;
    uint8_t *cur_vid = terminals[cur_term].vid_mem,
            *new_vid = terminals[new_term].vid_mem;
    for(i = 0; i < 2 * NUM_COLS * NUM_ROWS; i++) {
        *(cur_vid + i) = *(video_mem + i);
        *(video_mem + i) = *(new_vid + i);
    }
}
