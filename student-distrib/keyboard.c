/* Driver for the keyboard */

#include "keyboard.h"

/* The following array is taken from 
    http://www.osdever.net/bkerndev/Docs/keyboard.htm
   All credits where due
*/
static unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
  '9', '0', '-', '=', '\b', /* Backspace */
  '\t',         /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,          /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
 '\'', '`',   0,        /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',            /* 49 */
  'm', ',', '.', '/',   0,              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

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
 *   Function:  Gets kb scan code from data port when status register's input bit is set.
 *              Convert scan code into ASCII, then write it to screen.
 */
void keyboard_interrupt() {
    char c;
    
    // wait until status register indicates that data is ready to be read
    while(!(inb(STATUS_PORT) & INBUF_MASK)){
        // only output positive scan codes (key presses) and disregard neg ones (key release)
        if((c = inb(DATA_PORT)) >= 0) {
            // Testing rtc write
            if(c == 59) {
                // rtc_freq <<= 1;
                // if(rtc_freq > 0x400)
                    rtc_freq = 2;
                // printf("rtc_freq: %u", rtc_freq);
                rtc_write(0, &rtc_freq, 4);
            }
            else
                putc(keyboard_map[(unsigned char)c]);
        }
        
        break;
    }
    send_eoi(KEYBOARD_IRQ);
}

