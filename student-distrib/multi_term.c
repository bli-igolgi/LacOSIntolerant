/*
 * Code to be able to support multiple terminals
 */

#include "multi_term.h"
#include "syscalls.h"

static char* video_mem = (char *)VIDEO_ADDR;


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
        // Point the first terminal's video memory to regular video memory
        if(!i)
            terminals[i].vid_mem = (uint8_t *)(VIDEO_ADDR);
        else 
            terminals[i].vid_mem = (uint8_t *)(VIDEO_ADDR + (i+1)*FOUR_KB);
        terminals[i].scrn_c = terminals[i].scrn_r = 0;
        terminals[i].curs_r = terminals[i].curs_c = 0;
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
    // Make sure we switch to a terminal in range, and that we have room for this process
    if(new_term_id < 0 || new_term_id >= MAX_TERM_NUM ||
       our_popcount(pcb_status) >= MAX_PROCESSES)
        return;

    cli();
    switch_screen(new_term_id);

    switch_stackframe(new_term_id);
    sti();
}

/*
 * void switch_keyboard_and_cursor_pos(int new_term_id);
 *   Inputs: new_term_id - the ID of the terminal to switch to
 *   Return Value: none
 *   Function: Switches the current keyboard and cursor positions
 */
void switch_keyboard_and_cursor_pos(int new_term_id) {
    set_keyboard_pos(new_term_id, terminals[new_term_id].scrn_r, terminals[new_term_id].scrn_c);
    set_cursor_pos(new_term_id, terminals[new_term_id].curs_r, terminals[new_term_id].curs_c);
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
    // Put the visible memory terminal back to the "fake" video memory location
    terminals[vis_term_id].vid_mem = (uint8_t *)(VIDEO_ADDR + (vis_term_id+1)*FOUR_KB);
    // Multiply by 2 so the attribute part of video memory is also copied
    for(i = 0; i < 2 * NUM_COLS * NUM_ROWS; i++) {
        *(terminals[vis_term_id].vid_mem + i) = *(video_mem + i);
        *(video_mem + i) = *(terminals[new_term_id].vid_mem + i);
    }
    // Point the new terminal video memory to actual video memory
    terminals[new_term_id].vid_mem = (uint8_t *)VIDEO_ADDR;
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
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        :"=m"(terminals[vis_term_id].esp), "=m"(terminals[vis_term_id].ebp)
    );
    vis_term_id = new_term_id;
    sched_term_id = new_term_id;

    switch_keyboard_and_cursor_pos(new_term_id);

    cur_pcb = terminals[new_term_id].cur_task;
    // If this terminal hasn't been run yet, start shell on it
    if(!terminals[new_term_id].cur_task) {
        // Send end of interrupt for the keyboard
        send_eoi(1);
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
            : 
            : "r"(terminals[new_term_id].esp), "r"(terminals[new_term_id].ebp)
        );
    }
    return;
}
