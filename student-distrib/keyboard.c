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
// Flag for whether enter has been pressed
bool new_line = false;

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
    // wait until status register indicates that data is ready to be read
    while(!(inb(STATUS_PORT) & INBUF_MASK)) {
        c = inb(DATA_PORT);
        process_input(c);
        break;
    }
    send_eoi(KEYBOARD_IRQ);
}


/*
 * void process_input(char c);
 *   Inputs: c - the character to process
 *   Return Value: none
 *   Function: Performs different actions based on the key that was pressed/released
 */
void process_input(char c) {
    uint8_t c_print;
    // Positive scan codes (key down)
    if(c >= 0) {
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
            // Set the last character in the buffer to be the newline
            read_buf[read_buf_index < BUF_SIZE ? read_buf_index : BUF_SIZE-1] = '\n';
            putc('\n');
            new_line = true;
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
        // Test case 4, press CTRL+4
        else if(c == 0x05 && ctrl) {
            // Enable the RTC IRQ
            enable_irq(8);
            clear();
            clear_buffer();
            // Double the frequency
            rtc_freq <<= 1;
            if(rtc_freq > 0x400)
                rtc_freq = 2;
            rtc_write(0, &rtc_freq, 4);
        }
        // Test case 5, press CTRL+5
        else if(c == 0x06 && ctrl) {
            // Enable the RTC IRQ
            disable_irq(8);
            clear();
            clear_buffer();
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
        // printf("key_released: %d\n", c);
    }
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
