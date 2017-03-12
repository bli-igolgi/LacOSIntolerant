/* Driver for the system real time clock */

#include "rtc.h"

/* Initialize the rtc */
void rtc_init() {
    outb(0x70, 0x8B);           // select register B, and disable NMI
    char prev = inb(0x71);      // read the current value of register B
    outb(0x70, 0x8B);           // set the index again (a read will reset the index to register D)
    outb(0x71, prev | 0x40);    // write the previous value ORed with 0x40. This turns on bit 6 of register B

    // Enables the RTC on the slave's IRQ0
    enable_irq(RTC_IRQ);
}

/* When the rtc receives an interrupt call */
void rtc_interrupt() {
    printf("RTC interrupt occured");
    // test_interrupts();

    // Need to read from register C so that new interrupts can be processed
    outb(0x70, 0x0C);   // select register C
    inb(0x71);          // just throw away contents

    send_eoi(RTC_IRQ);
}
