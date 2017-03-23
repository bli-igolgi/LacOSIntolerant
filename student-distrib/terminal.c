/*
 * Driver for the terminal
 */

#include "terminal.h"


/*
 * int32_t terminal_open(const uint8_t* filename);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t terminal_open(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * int32_t terminal_close(int32_t fd);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
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
    memcpy(buf, read_buf, buf_size);

    clear_buffer();
    sti();
    return buf_size;
}

/*
 * int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd     - The keyboard file descriptor
 *           buf    - The data to write
 *           nbytes - How many bytes to write
 *   Return Value: The number of bytes written
 *   Function: Prints data to the screen
 */
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes) {
    cli();

    int b_written;
    b_written = printf((int8_t *)buf);

    sti();
    return b_written;
}
