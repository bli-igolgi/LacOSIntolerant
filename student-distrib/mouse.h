#ifndef RTC_H
#define RTC_H

#include "driver.h"
#include "i8259.h"

extern int32_t mouse_open(const uint8_t* filename);
extern int32_t mouse_close(int32_t fd);
extern int32_t mouse_read(int32_t fd, void *buf, int32_t nbytes);
extern int32_t mouse_write(int32_t fd, const void *buf, int32_t nbytes);

#endif
