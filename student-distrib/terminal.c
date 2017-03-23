/*
 * Driver for the terminal
 */

#include "terminal.h"


/*
 * int32_t terminal_open(const uint8_t* filename);
 *   Inputs: filename - pointer to the filename
 *   Return Value: 0 always
 *   Function: Opens the terminal
 */
int32_t terminal_open(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * int32_t terminal_close(int32_t fd);
 *   Inputs: fd - The terminal file descriptor
 *   Return Value: 0 always
 *   Function: Closes the terminal
 */
int32_t terminal_close(int32_t fd) {
    return SUCCESS;
}

/*
 * int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     - The keyboard file descriptor
 *           buf    - The data to read
 *           nbytes - How many bytes to read
 *   Return Value: The number of bytes read, or -1 if failed
 *   Function: 
 */
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes) {
    // Wait for enter to be pressed
    while(!new_line);
    cli();

    uint32_t buf_size = sizeof(read_buf);
    // Move the data entered since the last newline into the buf
    memcpy(buf, read_buf, buf_size);

    // Make the string null terminated
    ((int8_t *)buf)[read_buf_index] = '\0';

    // Clear the old keyboard data buffer
    clear_buffer();
    sti();
    return buf_size;
}

/*
 * int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd     - The keyboard file descriptor
 *           buf    - The data to write, a null terminated string
 *           nbytes - How many bytes to write
 *   Return Value: The number of bytes written
 *   Function: Prints data to the screen
 */
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes) {
    cli();

    int b_written;
    // Display the passed in data
    b_written = printf((int8_t *)buf);

    sti();
    return b_written;
}

/*
 * int32_t terminal_puts(int8_t* s);
 *   Inputs: s - pointer to a string of characters
 *   Return Value: Number of bytes written
 *   Function: Output a string to the console, terminated by the newline char
 */
int32_t terminal_puts(int8_t* s) {
    register int32_t index = 0;
    while(s[index] != '\n') {
        putc(s[index]);
        index++;
    }

    return index;
}
