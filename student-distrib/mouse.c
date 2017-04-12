/*
 * Driver for the mouse
 */

/* http://wiki.osdev.org/%228042%22_PS/2_Controller#PS.2F2_Controller_IO_Ports */

#include "mouse.h"

/*
 * void mouse_init(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: 
 */
void mouse_init() {
    outb(0xAE, 0x64);
    outb(0xF4, 0x60);
    enable_irq(MOUSE_IRQ);
}

/*
 * void mouse_interrupt(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: 
 */
void mouse_interrupt() {
    printf("mouse_interrupt occurred");
    send_eoi(MOUSE_IRQ);
}

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
