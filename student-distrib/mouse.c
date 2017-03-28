/*
 * Driver for the mouse
 */

#include "mouse.h"

/*
 * int32_t mouse_open(const uint8_t* filename);
 *   Inputs: filename 
 *   Return Value: 
 *   Function: Opens the mouse
 */
int32_t mouse_open(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * int32_t mouse_close(int32_t fd);
 *   Inputs: fd - 
 *   Return Value: 
 *   Function: Closes the mouse
 */
int32_t mouse_close(int32_t fd) {
    return SUCCESS;
}

/*
 * int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     - 
 *           buf    - 
 *           nbytes - 
 *   Return Value: 
 *   Function: 
 */
int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes) {
    return FAILURE;
}

/*
 * int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd     - 
 *           buf    - 
 *           nbytes - 
 *   Return Value: 
 *   Function: 
 */
int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes) {
    return FAILURE;
}
