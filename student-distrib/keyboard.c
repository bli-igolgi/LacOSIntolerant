/* Driver for the keyboard */

#include "keyboard.h"

// The codes for key presses
#define BACKSPACE_P 0X0E
#define ENTER_KEY_P 0x1C
#define CTRL_KEY_P  0x1D
#define L_KEY_P     0x26
#define L_SHIFT_P   0x2A
#define R_SHIFT_P   0x36
#define CAPS_LOCK_P 0x3A
#define ONE_KEY_P   0x02
#define TWO_KEY_P   0x03
#define THREE_KEY_P 0x04
#define FOUR_KEY_P  0x05
#define FIVE_KEY_P  0x06

// The codes for key releases
#define ENTER_KEY_R -100
#define CTRL_KEY_R  -99
#define L_SHIFT_R   -86
#define R_SHIFT_R   -74

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
    int last;
    // Positive scan codes (key down)
    if(c >= 0) {
        switch(c) {
            case BACKSPACE_P:
                // Don't backspace if nothing there
                if(read_buf_index) {
                    read_buf[--read_buf_index] = 0;
                    // If the character to remove is on the previous line
                    if(!screen_x) {
                        screen_y -= 1;
                        screen_x = NUM_COLS;
                    }
                    // Display the backspace
                    putc('\b');
                }
                break;
            case ENTER_KEY_P:
                // Set the last index to the last free spot in the array
                last = (read_buf_index < BUF_SIZE) ? read_buf_index : BUF_SIZE-1;
                // Set the last character in the buffer to be the newline
                read_buf[last] = '\n';
                // Null terminate the string
                read_buf[last+1] = '\0';
                putc('\n');
                new_line = true;
                break;
            case CTRL_KEY_P:
                ctrl = true;
                break;
            case L_KEY_P: 
                // CTRL+L
                if(ctrl) {
                    // Clears video memory
                    clear();
                    clear_buffer();
                    break;
                }
                // Treat it as a regular character
                else goto print_char;
            case L_SHIFT_P:
            case R_SHIFT_P:
                shift = true;
                break;
            case CAPS_LOCK_P:
                caps_lock = !caps_lock;
                break;
            case ONE_KEY_P:
                // Test case 1, press CTRL+1
                if(ctrl) {
                    // Print file name, file type, and file size for all files
                    test_access_by_index();
                }
                // Treat it as a regular character
                else goto print_char;
                break;
            case TWO_KEY_P:
                // Test case 2, press CTRL+2
                if(ctrl) {
                    // Read file by name (print name at end)
                    test_access_by_file_name();
                }
                // Treat it as a regular character
                else goto print_char;
                break;
            case THREE_KEY_P:
                // Test case 3, press CTRL+3
                if(ctrl) {
                    // Read file by index (print name at end)
                    test_data_printing();
                }
                // Treat it as a regular character
                else goto print_char;
                break;
            case FOUR_KEY_P:
                // Test case 4, press CTRL+4
                if(ctrl) {
                    // Enable the RTC IRQ
                    enable_irq(RTC_IRQ);
                    clear();
                    clear_buffer();
                    set_cursor_pos(0, 0);
                    // Double the frequency
                    rtc_freq <<= 1;
                    // Don't allow to go above 1024 Hz
                    if(rtc_freq > 0x400)
                        rtc_freq = 2;
                    rtc_write(0, &rtc_freq, 0);
                    break;
                }
                // Treat it as a regular character
                else goto print_char;
            case FIVE_KEY_P:
                // Test case 5, press CTRL+5
                if(ctrl) {
                    // Enable the RTC IRQ
                    disable_irq(RTC_IRQ);
                    clear();
                    clear_buffer();
                    set_cursor_pos(0, 0);
                    break;
                }
                // Treat it as a regular character
                else goto print_char;
            // Regular key press
            default:
print_char:
                if(read_buf_index == BUF_SIZE) return;

                c_print = get_keymap(c);
                // Don't print non-printing characters like F1
                if(c_print) {
                    read_buf[read_buf_index++] = c_print;
                    putc(c_print);
                }
                break;
        }
    }
    // Negative scan codes (key up)
    else {
        switch(c) {
            case CTRL_KEY_R:  ctrl = false;     break;
            case L_SHIFT_R:
            case R_SHIFT_R:   shift = false;    break;
            default: break;
        }
    }
}

/*
 * uint8_t get_keymap(char c);
 *   Inputs: c - the index into the array of the keymap
 *   Return Value: The character we desire
 *   Function: Checks the value of caps and shift to get
 *              the correct key out of the keymap
 */
uint8_t get_keymap(char c) {
    if(shift && caps_lock)  return caps_shift_key_map[(unsigned char)c];
    else if(shift)          return shift_key_map[(unsigned char)c];
    else if(caps_lock)      return caps_key_map[(unsigned char)c];
    else                    return reg_key_map[(unsigned char)c];
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
