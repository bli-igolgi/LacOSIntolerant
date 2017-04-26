/*
 * Driver for the Programmable Interrupt Timer
 */

// http://www.osdever.net/bkerndev/index.php

#include "timer.h"

static uint32_t timer_ticks = 0;

/*
 * void pit_init();
 *   Inputs: none
 *   Return Value: none
 *   Function: Initializes the PIT, and enables the IRQ line
 */
void pit_init() {
    // Set the frequency to 1000 Hz, so that interrupts can happen in milliseconds
    set_pit_freq(1000);
    enable_irq(PIT_IRQ);
}

/*
 * void set_pit_freq(uint32_t hz);
 *   Inputs: hz - what frequency to set the pit to (how often it will "interrupt")
 *   Return Value: none
 *   Function: Sets the PIT to receive interrupts hz times every second
 */
void set_pit_freq(uint32_t hz) {
    int divisor = BASE_FREQ / hz;          /* Calculate our divisor */
    outb(0x36, COMMAND_PORT);              /* Set our command byte 0x36 */
    outb(divisor & 0xFF, CHANNEL1_PORT);   /* Set low byte of divisor */
    outb(divisor >> 8, CHANNEL1_PORT);     /* Set high byte of divisor */
}

/*
 * void pit_interrupt();
 *   Inputs: none
 *   Return Value: none
 *   Function: PIT ISR
 */
void pit_interrupt() {
    cli();

    // Perform the task switch
    if(++timer_ticks >= TIME_QUANTUM) {
        // printf("%d milliseconds has passed\n", TIME_QUANTUM);
        timer_ticks = 0;
        // uint8_t next_tid = (cur_term_id + 1) % MAX_TERM_NUM;

        // if(terminals[next_tid].cur_task) switch_tasks(next_tid);
        // else if(terminals[(next_tid = ((next_tid + 1) % MAX_TERM_NUM))].cur_task)
        //     switch_tasks(next_tid);
    }

    send_eoi(PIT_IRQ);
    sti();
}

void switch_tasks(int new_term_id) {
    if(terminals[new_term_id].cur_task && new_term_id != cur_term_id) {
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

        map_page((void *)(cur_pcb->page_addr), (void *)PROGRAM_VIRT, true, true, true, true);
        flush_tlb();
        tss.esp0 = cur_pcb->esp0;
        tss.ss0 = cur_pcb->ss0;
        sti();
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
