/* Driver for the system real time clock */

#include "rtc.h"

/* Initialize the rtc */
void rtc_init() {
    // Enables the RTC on the slave's IRQ0
    enable_irq(8);
}

/* When the rtc receives an interrupt call */
void rtc_interrupt() {
    test_interrupts();
}
