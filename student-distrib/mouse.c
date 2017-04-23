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
    // Disable ps/2 ports
    /*outb(0xAD, STATUS_PORT);
    outb(0xA7, STATUS_PORT);

    // Flush output buffer
    inb(DATA_PORT);

    outb(0x20, STATUS_PORT);
    // Get response byte
    uint8_t ccb = inb(DATA_PORT);
    if((ccb >> 4) & 1) printf("double\n");*/


    // Enable first ps/2 port
    outb(0xAE, STATUS_PORT);
    // Enable second ps/2 port
    outb(0xA8, STATUS_PORT);
    // Enable reporting
    outb(0xF4, DATA_PORT);
    enable_irq(MOUSE_IRQ);
}

/*
 * void mouse_interrupt(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: 
 */
void mouse_interrupt() {
    // uint8_t packet = inb(DATA_PORT);
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
