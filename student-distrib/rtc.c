/*
 * Driver for the system real time clock
 */

#include "rtc.h"

// Global flag to indicate if the current interrupt cycle will be skipped
volatile bool intr_occured = false;
uint32_t rtc_freq = 0x400;

/*
 * void rtc_init(void);
 *   Inputs: void
 *   Return Value: none
 *   Function: Sets up RTC to allow interrupts to commence, unmasks
 *              the corresponding IRQ on the PIC
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
    cli();
    // 
    intr_occured = true;

    // Need to read from register C so that new interrupts can be processed
    outb(0x0C, CMOS_REG_1);   // select register C
    inb(CMOS_REG_2);          // just throw away contents

    send_eoi(RTC_IRQ);
    sti();
}

f_ops_table rtc_jt = { rtc_open, rtc_read, rtc_write, rtc_close };

/*
 * int32_t rtc_open(const uint8_t* filename);
 *   Inputs: 		filename - pointer to the filename
 *   Return Value:	-1 if no free file descriptors are available
 *					0-7 (fd) if it doesn't fail
 *   Function: 		Opens the RTC and sets initial interrupt frequency to 2
 */
int32_t rtc_open(const uint8_t* filename) {
	// Set initial RTC frequency to 2
    set_int_freq(0x0F);
	
    return open_file_desc(cur_pcb, rtc_jt, 0);		// RTC file inode # is 0
}

/*
 * int32_t rtc_close(int32_t fd);
 *   Inputs: fd - The RTC file descriptor
 *   Return Value: 
 *   Function: Closes the RTC
 */
int32_t rtc_close(int32_t fd) {
    return close_file_desc(cur_pcb, fd);
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
	intr_occured = false;
    // Wait for an interrupt to occur
    while(!intr_occured){}
	
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
    // Get the new frequency as the log of 2
    do {
        pow2freq++;
    } while((new_freq >>= 1) > 1);

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
 *              For example, if new_freq = 0xF, then
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
