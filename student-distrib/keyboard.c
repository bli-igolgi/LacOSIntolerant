/* Driver for the keyboard */

#include "keyboard.h"

// Key maps are defined in key_maps.c
extern unsigned char reg_key_map[128];
extern unsigned char shift_key_map[128];

// Flags for whether certain special keys are pressed
bool ctrl       = false,
     caps_lock  = false,
     num_lock   = false,
     shift      = false;

/*
 * void keyboard_init(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Sets up keyboard to allow interrupts to commence,
 *              and unmasks the corresponding IRQ on the PIC
 */
void keyboard_init() {
    enable_irq(KEYBOARD_IRQ);
}

/*
 * void keyboard_interrupt(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Gets kb scan code from data port when status register's input bit is set.
 *             Convert scan code into ASCII, then write it to screen.
 */
void keyboard_interrupt() {
    char c;
    
    // wait until status register indicates that data is ready to be read
    while(!(inb(STATUS_PORT) & INBUF_MASK)) {
        // Positive scan codes (key down)
        if((c = inb(DATA_PORT)) >= 0) {
            // Control
            if(c == 0x1D) {
                ctrl = true;
                break;
            }
            // L
            if(c == 0x26 && ctrl) {
                printf("\nCLEAR SCREEN NOW\n");
                break;
            }
            // Left or right shift
            if(c == 0x2A || c == 0x36) {
                shift = true;
                break;
            }
            // Testing rtc write, press F1
            if(c == 0x3B) {
                // Double the frequency
                rtc_freq <<= 1;
                if(rtc_freq > 0x400)
                    rtc_freq = 2;
                rtc_write(0, &rtc_freq, 4);
                break;
            }
            // Testing rtc read, press F2
            if(c == 0x3C) {
                rtc_read(0, &rtc_freq, 0);
                break;
            }

            if(shift)
                putc(shift_key_map[(unsigned char)c]);
            else
                putc(reg_key_map[(unsigned char)c]);
            // printf("key_pressed: %d\n", c);
        }
        // Negative scan codes (key up)
        else {
            switch(c) {
                // Control
                case -99:   ctrl = false;       break;
                // Left and right shift
                case -86:
                case -74:   shift = false;      break;
                default:
                    break;
            }
        }
        // printf("key_released: %d\n", c);
        
        break;
    }
    send_eoi(KEYBOARD_IRQ);
}

