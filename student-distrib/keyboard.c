/* Driver for the keyboard */

#include "keyboard.h"

/* 
Initializes the keyboard - should prob have a full function header
*/
void keyboard_init() {
    // Enables the keyboard on the master IRQ1's
    enable_irq(KEYBOARD_IRQ);
}

void keyboard_interrupt() {
    int c = inb(0x60);
    putc(c);
    send_eoi(KEYBOARD_IRQ);
}

void free_keyboard(){
	disable_irq(KEYBOARD_IRQ);
}