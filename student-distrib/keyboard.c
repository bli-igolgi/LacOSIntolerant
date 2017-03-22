/* Driver for the keyboard */

#include "keyboard.h"

// Key maps are defined in key_maps.c
extern unsigned char reg_key_map[KEY_MAP_SIZE];
extern unsigned char shift_key_map[KEY_MAP_SIZE];
extern unsigned char caps_key_map[KEY_MAP_SIZE];
extern unsigned char caps_shift_key_map[KEY_MAP_SIZE];

// Buffer for the input data from the keyboard
unsigned char read_buf[BUF_SIZE];
// Holds the location of the next char in the array
uint32_t read_buf_index = 0;

// Flags for whether certain special keys are pressed
bool ctrl       = false,
     caps_lock  = false,
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
    uint8_t c_print;
    
    // wait until status register indicates that data is ready to be read
    while(!(inb(STATUS_PORT) & INBUF_MASK)) {
        // Positive scan codes (key down)
        if((c = inb(DATA_PORT)) >= 0) {
            // Backspace
            if(c == 0x0E) {
                if(read_buf_index != 0) {
                    read_buf[--read_buf_index] = 0;
                    // If the character to remove is on the previous line
                    if(screen_x == 0) {
                        screen_y -= 1;
                        screen_x = NUM_COLS;
                    }
                    putc('\b');
                }
            }
            // Enter
            else if(c == 0x1C) {
                clear_buffer();
                putc('\n');
            }
            // Control
            else if(c == 0x1D)
                ctrl = true;
            // CTRL+L
            else if(c == 0x26 && ctrl) {
                // Clears video memory
                clear();
                clear_buffer();
            }
            // Left or right shift
            else if(c == 0x2A || c == 0x36)
                shift = true;
            // Caps lock
            else if(c == 0x3A)
                caps_lock = !caps_lock;
            // Testing rtc write, press CTRL+1
            else if(c == 0x02 && ctrl) {
                // Double the frequency
                rtc_freq <<= 1;
                if(rtc_freq > 0x400)
                    rtc_freq = 2;
                rtc_write(0, &rtc_freq, 4);
            }
            // Testing rtc read, press CTRL+2
            else if(c == 0x03 && ctrl) {
                rtc_read(0, &rtc_freq, 0);
            }
            // Regular key press
            else {
                if(read_buf_index == BUF_SIZE) return;
                // Get the correct keymap
                if(shift && caps_lock)  c_print = caps_shift_key_map[(unsigned char)c];
                else if(shift)          c_print = shift_key_map[(unsigned char)c];
                else if(caps_lock)      c_print = caps_key_map[(unsigned char)c];
                else                    c_print = reg_key_map[(unsigned char)c];

                read_buf[read_buf_index++] = c_print;
                putc(c_print);
            }
            // printf("key_pressed: %d\n", c);
        }
        // Negative scan codes (key up)
        else {
            switch(c) {
                // Control
                case -99: ctrl = false;   break;
                // Left and right shift
                case -86:
                case -74: shift = false;  break;
                default: break;
            }
        }
        // printf("key_released: %d\n", c);
        
        break;
    }
    send_eoi(KEYBOARD_IRQ);
}

/*
 * void clear_buffer(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Resets the read buffer
 */
void clear_buffer(void) {
    memset(read_buf, 0, sizeof(read_buf));
    read_buf_index = 0;
}
