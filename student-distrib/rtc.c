/*
 * Driver for the system real time clock
 */

#include "rtc.h"

/*
 * void rtc_init(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Sets up RTC to allow interrupts to commence, unmasks
 *              the corresponding IRQ on the PIC, and sets initial
 *              interrupt frequency to 2
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

    // Set initial RTC frequency to 2
    set_int_freq(0x0F);

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
    test_interrupts();

    // Need to read from register C so that new interrupts can be processed
    outb(0x0C, CMOS_REG_1);   // select register C
    inb(CMOS_REG_2);          // just throw away contents

    send_eoi(RTC_IRQ);
}

/*
 * Not sure this is needed
 */
int32_t rtc_open(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * Not sure this is needed
 */
int32_t rtc_close(int32_t fd) {
    return SUCCESS;
}

/*
 * int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes) {
    return SUCCESS;
}

/*
 * int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes) {
    // Get the new interrupt rate
    uint32_t new_freq = *((int *)buf),
             pow2freq = 0;
    // Make sure the new frequency is a power of 2 and is in range
    if(!(new_freq & (new_freq - 1)) || new_freq > 1024 || new_freq < 2)
        return FAILURE;

    // Get the new frequency as a power of 2
    while(new_freq >>= 1 != 1) pow2freq++;

    set_int_freq(pow2freq);

    return SUCCESS;
}

/*
 * void set_int_freq(int32_t new_freq);
 *   Inputs: new_freq - the value of the divisor of the new frequency
 *   Return Value: none
 *   Function: Writes a new frequency to the RTC.
 *              The frequency is calculated internally on the RTC by:
 *                  frequency = 32768 >> (new_freq - 1);
 *              Therefore, new_freq must be a power of 2.
 *
 *              For example, if new_freq = 0x0F, then
 *                  frequency = 32768 >> 14 = 2Hz
 */
void set_int_freq(int32_t new_freq) {
    // Select register A, and disable NMI
    outb(CONTROL_REG_A, CMOS_REG_1);
    // Get data from register A
    char prev = inb(CMOS_REG_2);
    // Sets the index again (a read will reset the index to register D)
    outb(CONTROL_REG_A, CMOS_REG_1);
    // write only our rate to A. Note, rate is the bottom 4 bits.
    outb(new_freq | (prev & 0xF0), CMOS_REG_2);
}
