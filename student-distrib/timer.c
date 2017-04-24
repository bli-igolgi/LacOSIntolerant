/*
 *
 */

// http://www.osdever.net/bkerndev/index.php

#include "timer.h"

/*
 * void pit_init();
 *   Inputs: none
 *   Return Value: none
 *   Function: Initializes the PIT, and enables the IRQ line
 */
void pit_init() {
    set_pit_freq(FREQUENCY);
    enable_irq(PIT_IRQ);
}

/*
 * void set_pit_freq(uint32_t hz);
 *   Inputs: hz - what frequency to set the pit to (how often it will "interrupt")
 *   Return Value: none
 *   Function: Sets the PIT to receive interrupts hz times every second
 */
void set_pit_freq(uint32_t hz) {
    int divisor = 1193180 / hz;            /* Calculate our divisor */
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

    printf("%d milliseconds has passed\n", TIME_QUANTUM);

    sti();
    send_eoi(PIT_IRQ);
}
