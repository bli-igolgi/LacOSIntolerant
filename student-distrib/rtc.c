/* Driver for the system real time clock */

#include "rtc.h"

/* Initialize the rtc */
void rtc_init() {
    // Enables the RTC on the slave's IRQ0
    enable_irq(RTC_IRQ);
}

/* When the rtc receives an interrupt call */
void rtc_interrupt() {
    printf("RTC interrupt occured");
    test_interrupts();

    outb(0x70, 0x0C);   // select register C
    inb(0x71);          // just throw away contents

    send_eoi(RTC_IRQ);
}
