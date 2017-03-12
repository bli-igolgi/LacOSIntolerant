/* Driver for the keyboard */

#include "keyboard.h"

/* Initializes the keyboard */
void keyboard_init() {
    // Enables the keyboard on the master IRQ1's
    enable_irq(1);
}

void keyboard_interrupt(char c) {
    printf("Keyboard interrupt occured with char %c", c);
}
