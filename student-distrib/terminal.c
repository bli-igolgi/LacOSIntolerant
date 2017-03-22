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
    cli();



    sti();
    return SUCCESS;
}

/*
 * int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t terminal_write(int32_t fd, const void *buf, int32_t nbytes) {
    return SUCCESS;
}
