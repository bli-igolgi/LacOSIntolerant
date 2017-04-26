/* Driver for the keyboard */

#include "keyboard.h"

// The codes for key presses
#define BACKSPACE_P 0X0E
#define ENTER_KEY_P 0x1C
#define CTRL_KEY_P  0x1D
#define L_KEY_P     0x26
#define L_SHIFT_P   0x2A
#define R_SHIFT_P   0x36
#define ALT_KEY_P   0x38
#define CAPS_LOCK_P 0x3A
#define F1_KEY_P    0x3B
#define F2_KEY_P    0x3C
#define F3_KEY_P    0x3D
#define UP_KEY_P    0x48
#define DOWN_KEY_P  0x50

// Number presses
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
// unsigned char read_buf[KEY_BUF_SIZE+1];
// Buffer for the last several commands
unsigned char hist_buf[HIST_COM_NUM][KEY_BUF_SIZE+1];
// Holds the location of the next char in the array
// uint32_t read_buf_index = 0;

// Holds the location of the next command to put in the array
uint32_t hist_buf_index = 0;
// Holds the location of the current history command index
uint32_t cur_hist_index = 0;

// Flag for whether enter has been pressed
bool new_line = false;

// Flags for whether certain special keys are pressed
bool ctrl       = false,
     alt        = false,
     caps_lock  = false,
     l_shift    = false,
     r_shift    = false;

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
        break;
    }
    process_input(c);
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
    int32_t buf_size, last, prev_size;
    static volatile bool rtc_loop;
    uint8_t *read_buf = terminals[vis_term_id].key_buf;
    uint32_t *read_buf_index = &terminals[vis_term_id].key_buf_index;
    // Positive scan codes (key down)
    if(c >= 0) {
        switch(c) {
            case BACKSPACE_P:
                // Don't backspace if nothing there
                if(*read_buf_index) {
                    read_buf[--(*read_buf_index)] = 0;
                    // If the character to remove is on the previous line
                    if(!screen_x) {
                        screen_y -= 1;
                        // x is decremented before putc_sched, so this is not an error
                        screen_x = NUM_COLS;
                    }
                    // Display the backspace
                    putc_sched('\b');
                }
                break;
            case ENTER_KEY_P:
                // Set the last index to the last free spot in the array
                last = (*read_buf_index < KEY_BUF_SIZE) ? *read_buf_index : KEY_BUF_SIZE-1;
                // Set the last character in the buffer to be the newline
                read_buf[last] = '\n';
                // Null terminate the string
                read_buf[last+1] = '\0';
                putc_sched('\n');
                // Reset the index so backspace doesn't go to previous line
                *read_buf_index = 0;
                // Tells terminal read that there is no more data to read
                new_line = true;
                break;
            case CTRL_KEY_P:
                ctrl = true;
                break;
            case ALT_KEY_P:
                alt = true;
                break;
            case L_KEY_P: 
                // CTRL+L
                if(ctrl) {
                    // Clears video memory
                    clear_screen();
                    clear_buffer();
                    break;
                }
                // Treat it as a regular character
                else goto print_char;
            case L_SHIFT_P:
                l_shift = true;
                break;
            case R_SHIFT_P:
                r_shift = true;
                break;
            case CAPS_LOCK_P:
                caps_lock = !caps_lock;
                break;

            /* Multiple Terminals */
            case F1_KEY_P:
            case F2_KEY_P:
            case F3_KEY_P:
                if(alt) {
                    int tid = c - F1_KEY_P;
                    // If already on the terminal that was selected, don't do anything
                    if(vis_term_id != tid)
                        switch_terminal(tid);
                }
                break;

            /* Command History */
            case UP_KEY_P:
                buf_size = strlen((int8_t *)hist_buf[cur_hist_index-1]);
                if(!buf_size) break;

                /* Clear any current input */
                clear_cur_cmd();
                clear_buffer();

                /* Copy the next history command into the read buffer and display it */
                memcpy(read_buf, (int8_t *)hist_buf[cur_hist_index-1], buf_size);
                cur_hist_index--;
                *read_buf_index = buf_size;
                printf((int8_t *)read_buf);

                break;
            case DOWN_KEY_P:
                prev_size = strlen((int8_t *)hist_buf[cur_hist_index]);
                buf_size = strlen((int8_t *)hist_buf[cur_hist_index+1]);

                /* Clear any current input */
                clear_cur_cmd();
                clear_buffer();

                // Allow input to be cleared if come to end of list
                if(!buf_size) {
                    if (prev_size != 0) {
                        cur_hist_index++;
                    }
                    break;
                }

                /* Copy the previous history command into the read buffer and display it */
                memcpy(read_buf, (int8_t *)hist_buf[cur_hist_index+1], buf_size);
                cur_hist_index++;
                *read_buf_index = buf_size;
                printf((int8_t *)read_buf);

                break;

            /* MP3.2 Test Cases */
            case FOUR_KEY_P:
                // Test case 4, press CTRL+4
                if(ctrl) {
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
                    rtc_loop = false;
                    // clear_screen();
                    // clear_buffer();
                    break;
                }
                // Treat it as a regular character
                else goto print_char;

            // Regular key press
            default:
print_char:
                if(*read_buf_index == KEY_BUF_SIZE) return;

                c_print = get_keymap(c);
                // Don't print non-printing characters like F1
                if(c_print) {
                    read_buf[(*read_buf_index)++] = c_print;
                    putc_vis(c_print);
                }
                break;
        }
    }
    // Negative scan codes (key up)
    else {
        switch(c) {
            case CTRL_KEY_R:  ctrl = false;     break;
            case L_SHIFT_R:   l_shift = false;  break;
            case R_SHIFT_R:   r_shift = false;  break;
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
    if((l_shift | r_shift) && caps_lock)    return caps_shift_key_map[(unsigned char)c];
    else if(l_shift | r_shift)              return shift_key_map[(unsigned char)c];
    else if(caps_lock)                      return caps_key_map[(unsigned char)c];
    else                                    return reg_key_map[(unsigned char)c];
}

/*
 * void clear_cur_cmd();
 *   Inputs: none
 *   Return Value: none
 *   Function: Clears the command currently displayed on the screen,
 *              as well as the read buffer holding the command
 */
void clear_cur_cmd() {
    int char_cnt = terminals[vis_term_id].key_buf_index;
    // Clear the screen
    while(char_cnt-- > 0)
        process_input(BACKSPACE_P);

    // Clear the buffer
    clear_buffer();
}

/*
 * void add_to_history(int8_t *command);
 *   Inputs: command - the command to add to the history buffer
 *   Return Value: none
 *   Function: Put the last command into the history buffer,
 *              if it exists and isn't the same as the previous command
 */
void add_to_history(int8_t *command) {
    // TODO: Need to implement data shifting so new commands are placed in array
    if(hist_buf_index >= HIST_COM_NUM) return;
    // Don't include the newline character
    int32_t buf_size = strlen((int8_t *)command)-1;
    // Put the last command into the history buffer,
    // if it exists and isn't the same as the previous command
    if(buf_size && strncmp((int8_t *)hist_buf[hist_buf_index-1], command, buf_size)) {
        strncpy((int8_t *)hist_buf[hist_buf_index++], command, buf_size);
    }
    cur_hist_index = hist_buf_index;
}

/*
 * void clear_buffer();
 *   Inputs: void
 *   Return Value: none
 *   Function: Resets the read buffer
 */
void clear_buffer() {
    memset(terminals[vis_term_id].key_buf, 0, strlen((int8_t *)terminals[vis_term_id].key_buf));
    terminals[vis_term_id].key_buf_index = 0;
}
