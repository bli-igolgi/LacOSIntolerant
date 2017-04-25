/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"
#include "syscalls.h"

static char* video_mem = (char *)VIDEO_ADDR;

term_t terminals[MAX_TERM_NUM];
int cur_term_id = 0;

/*
 * void multi_term_init();
 *   Inputs: none
 *   Return Value: none
 *   Function: Initializes the array of terminals
 */
void multi_term_init() {
    uint32_t i;
    for(i = 0; i < MAX_TERM_NUM; i++) {
        terminals[i].term_id = i;
        terminals[i].vid_mem = (uint8_t *)(VIDEO_ADDR + (i+1)*FOUR_KB);
        terminals[i].key_x = terminals[i].key_y = 0;
        terminals[i].curs_x = terminals[i].curs_y = 0;
        terminals[i].key_buf_index = terminals[i].esp = terminals[i].ebp = 0;
        terminals[i].cur_task = NULL;
    }
    cur_pcb = terminals[0].cur_task;
}

/*
 * void switch_terminal(int new_term_id);
 *   Inputs: new_term_id - the ID of the terminal to switch to
 *   Return Value: none
 *   Function: Switches from the current terminal to the new terminal,
 *               updating video memory and switching to the other process
 */
void switch_terminal(int new_term_id) {
    if(new_term_id < 0 || new_term_id >= MAX_TERM_NUM) return;
    // Clear interrupts to prevent terminal switching to happen again
    cli();
    switch_screen(new_term_id);

    switch_keyboard_and_cursor_pos(new_term_id);

    switch_stackframe(new_term_id);
}

/*
 * void switch_keyboard_and_cursor_pos(int new_term_id);
 *   Inputs: new_term_id - the ID of the terminal to switch to
 *   Return Value: none
 *   Function: Saves and switches the current keyboard and cursor positions
 */
void switch_keyboard_and_cursor_pos(int new_term_id) {
    terminals[cur_term_id].key_x = screen_x;
    terminals[cur_term_id].key_y = screen_y;
    terminals[cur_term_id].curs_x = cursor_x;
    terminals[cur_term_id].curs_y = cursor_y;

    set_keyboard_pos(terminals[new_term_id].key_x, terminals[new_term_id].key_y);
    set_cursor_pos(terminals[new_term_id].curs_x, terminals[new_term_id].curs_y);
}

/*
 * void switch_screen(int new_term_id);
 *   Inputs: new_term_id - the ID of the terminal to switch to
 *   Return Value: none
 *   Function: Copies video memory into the current terminal,
 *              and copies the new terminal's old screen to video memory 
 */
void switch_screen(int new_term_id) {
    int32_t i;
    // Multiply by 2 so the attribute part of video memory is also copied
    for(i = 0; i < 2 * NUM_COLS * NUM_ROWS; i++) {
        *(terminals[cur_term_id].vid_mem + i) = *(video_mem + i);
        *(video_mem + i) = *(terminals[new_term_id].vid_mem + i);
    }
}

/*
 * void switch_stackframe(int new_term_id);
 *   Inputs: new_term_id - the ID of the terminal to switch to
 *   Return Value: none
 *   Function: Switches the stack frame to the new terminal
 */
void switch_stackframe(int new_term_id) {
    // Push registers and save the esp and ebp
    asm volatile(
        //"pushal;"
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=r"(terminals[cur_term_id].esp), "=r"(terminals[cur_term_id].ebp)
    );
    cur_term_id = new_term_id;
    // If this terminal hasn't been run yet, start shell on it
    cur_pcb = terminals[new_term_id].cur_task;
    if(!terminals[new_term_id].cur_task) {
        // Send end of interrupt for the keyboard
        send_eoi(1);
        sti();
        sys_execute((uint8_t *)"shell");
    }
    else {
        map_page((void *)(cur_pcb->page_addr), (void *)PROGRAM_VIRT, true, true, true, true);
        flush_tlb();
        tss.esp0 = cur_pcb->esp0;
        tss.ss0 = cur_pcb->ss0;
        // Move the old stack frame back and pop registers to return to old task
        asm volatile(
            "movl %0, %%esp;"
            "movl %1, %%ebp;"
            //"popal"
            ""
            : 
            : "r"(terminals[new_term_id].esp), "r"(terminals[new_term_id].ebp)
        );
    }
    return;
}
