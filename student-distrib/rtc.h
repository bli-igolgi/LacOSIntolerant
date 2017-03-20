#ifndef RTC_H
#define RTC_H

#include "driver.h"
#include "i8259.h"

// The IRQ number on the PIC
#define RTC_IRQ 		8
// The control register A for RTC
#define CONTROL_REG_A   0x8A
// The control register B for RTC
#define CONTROL_REG_B   0x8B
// Enable bit 6 to turn on RTC
#define BIT_6			0x40
// The registers that data is put on for RTC
#define CMOS_REG_1 		0x70
#define CMOS_REG_2 		0x71


extern void rtc_init();
extern void rtc_interrupt();

// Not sure this is needed
extern int32_t rtc_open(const uint8_t* filename);
// Not sure this is needed
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);

// Writes the new frequency to the RTC port
void set_int_freq(int32_t new_freq);

#endif
