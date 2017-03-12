#ifndef RTC_H
#define RTC_H

#include "lib.h"
#include "i8259.h"

#define RTC_IRQ 8

extern void rtc_init();
extern void rtc_interrupt();

#endif
