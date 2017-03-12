#ifndef RTC_H
#define RTC_H

#include "lib.h"
#include "i8259.h"

// The IRQ number on the PIC
#define RTC_IRQ 		8
// The control register B for RTC
#define CONTROL_REG_B 	0x8B
// Enable bit 6 to turn on RTC
#define BIT_6			0x40
// The registers that data is put on for RTC
#define CMOS_REG_1 		0x70
#define CMOS_REG_2 		0x71


extern void rtc_init();
extern void rtc_interrupt();

#endif
