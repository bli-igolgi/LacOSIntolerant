/* Driver for the system real time clock */

#include "rtc.h"

/*
 * void rtc_init(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Sets up RTC to allow interrupts to commence,
 *              and unmasks the corresponding IRQ on the PIC
 */
void rtc_init() {
    // Select register B, and disable NMI
    outb(CONTROL_REG_B, CMOS_REG_1);
    // Get data from register B
    char prev = inb(CMOS_REG_2);
    // Sets the index again (a read will reset the index to register D)
    outb(CONTROL_REG_B, CMOS_REG_1);
    // Write previous value with bit 6 enabled to turn on interrupts
    outb(prev | BIT_6, CMOS_REG_2);

    // Enables the RTC on the slave's IRQ0
    enable_irq(RTC_IRQ);
}

/*
 * void rtc_interrupt(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: RTC interrupt handler, also clears the RTC register
 *              to allow interrupts to continue executing
 */
void rtc_interrupt() {
    // printf("RTC interrupt occured");
    test_interrupts();

    // Need to read from register C so that new interrupts can be processed
    outb(0x0C, CMOS_REG_1);   // select register C
    inb(CMOS_REG_2);          // just throw away contents

    send_eoi(RTC_IRQ);
}
