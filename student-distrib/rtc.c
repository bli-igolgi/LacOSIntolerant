/*
 * Driver for the system real time clock
 */

#include "rtc.h"

// Global flag to indicate if the current interrupt cycle will be skipped
static volatile bool skip_interrupt = false;
uint32_t rtc_freq = 0x02;

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
    // enable_irq(RTC_IRQ);
}

/*
 * void rtc_interrupt(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: RTC interrupt handler, also clears the RTC register
 *              to allow interrupts to continue executing
 */
void rtc_interrupt() {
    // If this interrupt cycle is going to skip
    if(skip_interrupt == true) {
        skip_interrupt = false;
    }
    // Otherwise, perform the following work
    else {
        test_interrupts();
    }

    // Need to read from register C so that new interrupts can be processed
    outb(0x0C, CMOS_REG_1);   // select register C
    inb(CMOS_REG_2);          // just throw away contents

    send_eoi(RTC_IRQ);
}

/*
 * 
 */
int32_t rtc_open(const uint8_t* filename) {
    return FAILURE;
}

/*
 * 
 */
int32_t rtc_close(int32_t fd) {
    return FAILURE;
}

/*
 * int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     - The RTC file descriptor
 *           buf    - unused
 *           nbytes - How many bytes to write
 *   Return Value: 0 after a cycle has been skipped
 *   Function: Skips an interrupt cycle
 */
int32_t rtc_read(int32_t fd, void *buf, int32_t nbytes) {
    sti();
    skip_interrupt = true;
    int d = 0;
    while(skip_interrupt == true) {
        printf("waiting: %d\n",d++);
    }
    printf("ONE CYCLE WAS SKIPPED\n");
    return SUCCESS;
}

/*
 * int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd     - The RTC file descriptor
 *           buf    - The new rate to set the RTC to
 *           nbytes - How many bytes to write
 *   Return Value: The number of bytes written, or -1 if failed
 *   Function: Writes a new interrupt rate to the RTC controller
 */
int32_t rtc_write(int32_t fd, const void *buf, int32_t nbytes) {
    // Get the new interrupt rate
    uint32_t new_freq = *((int *)buf),
             pow2freq = 0;

    // Make sure the new frequency is a power of 2 and is in range
    if(new_freq & (new_freq - 1) || new_freq > 1024 || new_freq < 2)
        return FAILURE;

    rtc_freq = new_freq;
    // Get the new frequency as a power of 2
    do {
        pow2freq++;
    } while((new_freq >>= 1) != 1);

    // Invert the new frequency value
    set_int_freq(0x10 - pow2freq);

    // The number of bytes written
    return nbytes;
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
    cli();

    // Select register A, and disable NMI
    outb(CONTROL_REG_A, CMOS_REG_1);
    // Get data from register A
    char prev = inb(CMOS_REG_2);
    // Sets the index again (a read will reset the index to register D)
    outb(CONTROL_REG_A, CMOS_REG_1);
    // Put the frequency divider in the bottom 4 bits of CRA
    outb(new_freq | (prev & 0xF0), CMOS_REG_2);

    sti();
}
