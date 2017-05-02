#ifndef MOUSE_H
#define MOUSE_H

#include "driver.h"
#include "i8259.h"

#define MOUSE_IRQ 12

// PS/2 ports
#define STATUS_PORT 0x64
#define DATA_PORT   0x60

void mouse_init(void);
void mouse_interrupt(void);

void mouse_wait(bool type);

int32_t mouse_open(const uint8_t* filename);
int32_t mouse_close(int32_t fd);
// extern int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes);
// extern int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes);
uint8_t mouse_read();
void mouse_write(uint8_t a_write);

extern int8_t mouse_x_pos, mouse_y_pos;

#endif
