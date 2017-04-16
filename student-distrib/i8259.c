/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */


/*
 * void i8259_init(void);
 *   Inputs: none
 *   Return Value: none
 *   Function: Sends the control words to master and slave PICs
 */
void
i8259_init(void) {
    // Set up the master's control words
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_PORT_2);
    outb(ICW3_MASTER, MASTER_8259_PORT_2);
    outb(ICW4, MASTER_8259_PORT_2);

    // Set up the slave's control words
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT_2);
    outb(ICW3_SLAVE, SLAVE_8259_PORT_2);
    outb(ICW4, SLAVE_8259_PORT_2);
	
    // Disable all interrupts and set both intr masks to off
    outb(OFF_MASK, MASTER_8259_PORT_2);
    outb(OFF_MASK, SLAVE_8259_PORT_2);
    master_mask = OFF_MASK;
    slave_mask = OFF_MASK;
	
	// Enable the slave IRQ port (IRQ2) to allow slave interrupts as needed
    enable_irq(2);
}

/*
 * void enable_irq(uint32_t irq_num);
 *   Inputs: irq_num = number of the irq to enable
 *   Return Value: none
 *   Function: Unmasks the IQR number
 */
void
enable_irq(uint32_t irq_num) {
    // Master interrupt
    if(irq_num <= MASTER_IRQ_NUM) {
        master_mask &= ~(1 << irq_num);
        outb(master_mask, MASTER_8259_PORT_2);
    }
    // Slave interrupt
    else if(irq_num <= SLAVE_IRQ_NUM) {
        // Put slave irqs in 0-7 range
        slave_mask &= ~(1 << (irq_num - 8));
        outb(slave_mask, SLAVE_8259_PORT_2);
    }
    // requested irq outside valid range
    else
        printf("Valid range: IRQ0 - IRQ15. Check yo'self befo' you rekt yo'self.\n");
}

/*
 * void disable_irq(uint32_t irq_num);
 *   Inputs: irq_num = number of the irq to disable
 *   Return Value: none
 *   Function: Masks the IQR number
 */
void
disable_irq(uint32_t irq_num) {
    // Master interrupt
    if(irq_num <= MASTER_IRQ_NUM) {
        master_mask |= 1 << irq_num;
        outb(master_mask, MASTER_8259_PORT_2);
    }
    // Slave interrupt
    else if(irq_num <= SLAVE_IRQ_NUM) {
        // Put slave irqs in 0-7 range
        slave_mask |= 1 << (irq_num - 8);
        outb(slave_mask, SLAVE_8259_PORT_2);
    }
    else
        printf("Nothing to disable. Why are you like this?\n");
}

/*
 * void send_eoi(void);
 *   Inputs: none
 *   Return Value: none
 *   Function: Send end-of-interrupt signal for the specified IRQ
 */
void
send_eoi(uint32_t irq_num) {
    // Master interrupt
    if(irq_num <= MASTER_IRQ_NUM) outb(irq_num | EOI, MASTER_8259_PORT);
    // Slave interrupt
    else {
        // Put slave irqs in 0-7 range, and enable this interrupt
        outb((irq_num - 8) | EOI, SLAVE_8259_PORT);
        // Enable the IRQ2 on master
        outb(2 | EOI, MASTER_8259_PORT);
    }
}
