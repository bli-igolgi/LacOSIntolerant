#ifndef MOUSE_H
#define MOUSE_H

#include "driver.h"
#include "i8259.h"

#define MOUSE_IRQ 12

extern void mouse_init(void);
extern void mouse_interrupt(void);

extern int32_t mouse_open(const uint8_t* filename);
extern int32_t mouse_close(int32_t fd);
extern int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes);
extern int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes);

#endif
