/*
 * Driver for the mouse
 * Code adapted from http://houbysoft.com/download/ps2mouse.html
 */

#include "mouse.h"

static int cycle = 0;
static uint8_t mouse_bytes[3];
static bool x_neg = false, y_neg = false;
int8_t mouse_x_pos = 0, mouse_y_pos = 0;

/*
 * void mouse_init(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Sets up the mouse to be able to receive interrupts
 */
void mouse_init() {
    mouse_wait(1);
    // Enable second ps/2 port
    outb(0xA8, STATUS_PORT);

    mouse_wait(1);
    // Tell the mouse to put the status byte in the data port
    outb(0x20, STATUS_PORT);

    unsigned char status_byte;
    mouse_wait(0);
    // Enable interrupts
    status_byte = (inb(DATA_PORT) | 2);

    mouse_wait(1);
    // Tell the controller we are sending a status byte
    outb(0x60, STATUS_PORT);

    mouse_wait(1);
    outb(status_byte, DATA_PORT);

    // Use the default settings
    mouse_write(0xF6);
    mouse_read();

    // Enable the mouse
    mouse_write(0xF4);
    mouse_read();

    enable_irq(MOUSE_IRQ);
}

/*
 * void mouse_interrupt(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: 
 */
void mouse_interrupt() {
    int16_t delta_x = 0, delta_y = 0;
    mouse_bytes[cycle++] = inb(DATA_PORT);

    send_eoi(MOUSE_IRQ);

    if (cycle == 3) { // if we have all the 3 bytes...
        cycle = 0;

        // If mouse only sends overflow information
        if ((mouse_bytes[1] & 0x80) || (mouse_bytes[1] & 0x40)) return;

        // Indicates if delta-y is towards the top
        if (!(mouse_bytes[1] & 0x20)) y_neg = true;
        else y_neg = false;
        // Indicates if delta-x is towards the left
        if (mouse_bytes[1] & 0x10) x_neg = true;
        else x_neg = false;

        // Middle click
        if (mouse_bytes[1] & 0x4)
            printf("Middle button is pressed!\n");
        // Right click
        if (mouse_bytes[1] & 0x2)
            printf("Right button is pressed!\n");
        // Left click
        if (mouse_bytes[1] & 0x1)
            printf("Left button is pressed!\n");
        
        // X Movement
        if((delta_x = (mouse_bytes[2] & 1))) {
            if(x_neg) delta_x = -delta_x;
            mouse_x_pos += delta_x;
            if(mouse_x_pos < 0) mouse_x_pos = 0;
            if(mouse_x_pos >= NUM_COLS) mouse_x_pos = NUM_COLS-1;
        }
        // Y Movement
        if((delta_y = (mouse_bytes[0] & 1))) {
            if(y_neg) delta_y = -delta_y;
            mouse_y_pos += delta_y;
            if(mouse_y_pos < 0) mouse_y_pos = 0;
            if(mouse_y_pos >= NUM_ROWS) mouse_y_pos = NUM_ROWS-1;
        }
        printf("mouse_x: %d, mouse_y: %d\n", mouse_x_pos, mouse_y_pos);
    }
}


/*
 * void mouse_wait(unsigned char type);
 *   Inputs: type - 1 if waiting to write command, 0 if waitiing to receive command
 *   Return Value: 
 *   Function: Waits for the mouse to be able to send or receive data
 */
void mouse_wait(bool type) {
    uint32_t _time_out = 100000;
    if(!type) {
        while(_time_out--) { // Data
            if((inb(STATUS_PORT) & 1) == 1) return;
        }
        return;
    }
    else {
        while(_time_out--) { // Signal
            if((inb(STATUS_PORT) & 2) == 0) return;
        }
        return;
    }
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
 *   Inputs: fd
 *   Return Value: 
 *   Function: Closes the mouse
 */
int32_t mouse_close(int32_t fd) {
    return SUCCESS;
}

/*
 * uint8_t mouse_read();
 *   Inputs: none
 *   Return Value: The data from the mouse
 *   Function: Waits for the mouse, then returns the data
 */
uint8_t mouse_read() {
    // Get response from mouse
    mouse_wait(0);
    return inb(DATA_PORT);
}

/*
 * void mouse_write(uint8_t a_write);
 *   Inputs: a_write - The command to send
 *   Return Value: none
 *   Function: Wait for the mouse, then send a command
 */
void mouse_write(uint8_t a_write) {
    // Wait to be able to send a command
    mouse_wait(1);
    // Tell the mouse we are sending a command
    outb(0xD4, STATUS_PORT);
    // Wait for the final part
    mouse_wait(1);
    // Finally write
    outb(a_write, DATA_PORT);
}

/*
 * int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     - 
 *           buf    - 
 *           nbytes - 
 *   Return Value: 
 *   Function: 
 */
// int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes) {
//     return FAILURE;
// }

/*
 * int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd     - 
 *           buf    - 
 *           nbytes - 
 *   Return Value: 
 *   Function: 
 */
// int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes) {
//     return FAILURE;
// }
