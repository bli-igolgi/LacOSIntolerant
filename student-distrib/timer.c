/*
 * Driver for the Programmable Interrupt Timer
 */

#include "timer.h"

/*
 * void pit_init();
 *   Inputs: none
 *   Return Value: none
 *   Function: Initializes the PIT, and enables the IRQ line
 */
void pit_init() {
    set_pit_freq(PIT_FREQ);
    enable_irq(PIT_IRQ);
}

/*
 * void set_pit_freq(uint32_t hz);
 *   Inputs: hz - what frequency to set the pit to (how often it will "interrupt")
 *   Return Value: none
 *   Function: Sets the PIT to receive interrupts hz times every second
 */
void set_pit_freq(uint32_t hz) {
    int divisor = BASE_FREQ / hz;           // Calculate the divisor
    outb(0x36, COMMAND_PORT);               // Put the PIT in Square Wave Mode, select channel 0, and transfer 16 bit divisor
    outb(divisor & 0xFF, CHANNEL1_PORT);    // Set low byte of divisor
    outb(divisor >> 8, CHANNEL1_PORT);      // Set high byte of divisor
}

/*
 * void pit_interrupt();
 *   Inputs: none
 *   Return Value: none
 *   Function: Interrupt handler for the PIT
 */
void pit_interrupt() {
    cli();

    send_eoi(PIT_IRQ);
    uint8_t next_tid = (sched_term_id + 1) % MAX_TERM_NUM;
    switch_tasks(next_tid);

    sti();
}

/*
 * void switch_tasks(int new_term_id);
 *   Inputs: new_term_id - the task to switch to
 *   Return Value: none
 *   Function: Switches to the current task (PCB) located in
 *              the terminal corresponding to the specified index
 */
void switch_tasks(int new_term_id) {
    if(new_term_id != sched_term_id) {
        // Push registers and save the esp and ebp
        asm volatile(
            "movl %%esp, %0;"
            "movl %%ebp, %1;"
            :"=r"(terminals[sched_term_id].esp), "=r"(terminals[sched_term_id].ebp)
        );
        sched_term_id = new_term_id;
        cur_pcb = terminals[new_term_id].cur_task;

        // If this terminal hasn't been run yet, start shell on it
        if(!cur_pcb) {
            starting_new_shell = true;
            sys_execute((uint8_t *)"shell");
        }
        else {
            starting_new_shell = false;
            map_page((void *)(cur_pcb->page_addr), (void *)PROGRAM_VIRT, true, true, true, true);
            // Map the vidmap address to the correct terminal video memory
            map_page((void *)terminals[new_term_id].vid_mem, (void *)VIDMAP_VIRT_ADDR, false, true, true, true);

            flush_tlb();
            tss.esp0 = cur_pcb->esp0;
            // Move the old stack frame back and pop registers to return to old task
            asm volatile(
                "movl %0, %%esp;"
                "movl %1, %%ebp;"
                : 
                : "r"(terminals[new_term_id].esp), "r"(terminals[new_term_id].ebp)
            );
        }
    }
    return;
}
