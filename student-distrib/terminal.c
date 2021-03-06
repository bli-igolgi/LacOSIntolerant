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
    char* standard_input = "stdin", *standard_output = "stdout";
	open_file_desc(cur_pcb, (uint8_t *)standard_input);
    open_file_desc(cur_pcb, (uint8_t *)standard_output);
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
    while(!terminals[sched_term_id].new_line);
    cli();
    terminals[sched_term_id].new_line = false;

    int32_t buf_size = strlen((int8_t *)terminals[sched_term_id].key_buf);
    // Move the data entered since the last newline into the buf
    memcpy(buf, terminals[sched_term_id].key_buf, nbytes);
    add_to_history((int8_t *)terminals[sched_term_id].key_buf);

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

    int i;
    // Display the passed in data
    for(i = 0; i < nbytes; i++)
        putc(sched_term_id,*((uint8_t *)buf + i));
    sti();
    return nbytes;
}
