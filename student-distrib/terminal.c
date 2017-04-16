/*
 * Driver for the terminal
 */

#include "terminal.h"

// Section below is file_ops jump tables for keyboard & terminal
f_ops_table stdin = { NULL, terminal_read, NULL, NULL };     // read-only
f_ops_table stdout = { NULL, NULL, terminal_write, NULL };   // write-only

/*
 * int32_t terminal_open(const uint8_t* filename);
 *   Inputs: filename - junk pointer
 *   Return Value: 0 always
 *   Function: Opens the fd associated with stdin & stdout on terminal
 */
int32_t terminal_open(const uint8_t* filename) {
	open_file_desc(cur_pcb, stdin, 0);
    open_file_desc(cur_pcb, stdout, 0);
	return SUCCESS;		// this function should never fail to find free fd...
}

/*
 * int32_t terminal_close(int32_t fd);
 *   Inputs: fd - The terminal file descriptor
 *   Return Value: -1 always
 *   Function: Closes the terminal
 */
int32_t terminal_close(int32_t fd) {
    return FAILURE;		// this function should never be called
}

/*
 * int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     - The keyboard file descriptor
 *           buf    - The buffer to read the data into
 *           nbytes - How many bytes to read
 *   Return Value: The number of bytes read, or -1 if failed
 *   Function: 
 */
int32_t terminal_read(int32_t fd, void *buf, int32_t nbytes) {
    // Wait for enter to be pressed
    while(!new_line);
    cli();
    new_line = false;

    int32_t buf_size = strlen((int8_t *)read_buf);
    // Move the data entered since the last newline into the buf
    memcpy(buf, read_buf, buf_size);

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
    b_written = printf("%s", (uint8_t *)buf);

    sti();
    return b_written;
}
