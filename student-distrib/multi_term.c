/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"
#include "syscalls.h"

static char* video_mem = (char *)VIDEO_ADDR;

term_t terminals[MAX_TERM_NUM];
int cur_term_id = 0;

void multi_term_init() {
    uint32_t i;
    for(i = 0; i < MAX_TERM_NUM; i++){
        terminals[i].term_id = i;
        terminals[i].vid_mem = (uint8_t *)(VIDEO_ADDR + (i+1)*FOUR_KB);
        terminals[i].key_x = terminals[i].key_y = 0;
        terminals[i].curs_x = terminals[i].curs_y = 0;
        terminals[i].key_buf_index = terminals[i].esp = terminals[i].ebp = 0;
        terminals[i].cur_task = NULL;
    }
}

void switch_terminal(int new_term_id) {
    terminals[cur_term_id].key_x = screen_x;
    terminals[cur_term_id].key_y = screen_y;
    terminals[cur_term_id].curs_x = cursor_x;
    terminals[cur_term_id].curs_y = cursor_y;

    switch_screen(new_term_id);
    set_keyboard_pos(terminals[new_term_id].key_x, terminals[new_term_id].key_y);
    set_cursor_pos(terminals[new_term_id].curs_x, terminals[new_term_id].curs_y);

    switch_stackframe(new_term_id);
}

void switch_screen(int new_term_id) {
    int32_t i;
    uint8_t *cur_vid = terminals[cur_term_id].vid_mem,
            *new_vid = terminals[new_term_id].vid_mem;
    for(i = 0; i < 2 * NUM_COLS * NUM_ROWS; i++) {
        *(cur_vid + i) = *(video_mem + i);
        *(video_mem + i) = *(new_vid + i);
    }
}

void switch_stackframe(int new_term_id) {
    asm volatile(
        "pushal;"
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=m"(terminals[cur_term_id].esp), "=m"(terminals[cur_term_id].ebp)
    );
    cur_term_id = new_term_id;
    if(!terminals[new_term_id].cur_task) {
        // Send end of interrupt for the keyboard
        send_eoi(1);
        sys_execute((uint8_t *)"shell");
    }
    else {
        asm volatile(
            "movl %0, %%esp;"
            "movl %1, %%ebp;"
            "popal"
            ""
            : 
            : "m"(terminals[new_term_id].esp), "m"(terminals[new_term_id].ebp)
        );
    }
    return;
}
