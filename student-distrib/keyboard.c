/* Driver for the keyboard */

#include "keyboard.h"

/* Initializes the keyboard */
void keyboard_init() {
    // Enables the keyboard on the master IRQ1's
    enable_irq(KEYBOARD_IRQ);
}

void keyboard_interrupt(char d) {
    printf("Keyboard interrupt occured with char %c", d);
    int c = inb(0x60);
    putc(c);
    send_eoi(KEYBOARD_IRQ);
}
