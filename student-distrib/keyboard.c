/* Driver for the keyboard */

#include "keyboard.h"

/* Initializes the keyboard */
void keyboard_init() {
    // Enables the RTC on the slave's IRQ0
    enable_irq(1);
}

void keyboard_interrupt() {

}
