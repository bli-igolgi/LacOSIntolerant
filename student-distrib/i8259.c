/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void) {
    // Disable all interrupts
    outb(MASK, MASTER_8259_PORT_2);
    outb(MASK, SLAVE_8259_PORT_2);

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
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num) {
    // printf("Enabling IRQ%d\n", irq_num);
    // Put the mask at the correct location
    uint32_t mask = ~(1 << irq_num);
    // Master interrupt
    if(irq_num < 8) {
        master_mask &= mask;
        outb(master_mask, MASTER_8259_PORT_2);
    }
    // Slave interrupt
    else {
        slave_mask &= mask;
        outb(slave_mask, SLAVE_8259_PORT_2);
    }
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num) {
    // printf("Disabling IRQ%d\n", irq_num);
    // Put the mask at the correct location
    uint32_t mask = 1 << irq_num;
    // Master interrupt
    if(irq_num < 8) {
        master_mask |= mask;
        outb(master_mask, MASTER_8259_PORT_2);
    }
    // Slave interrupt
    else {
        slave_mask |= mask;
        outb(slave_mask, SLAVE_8259_PORT_2);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num) {
    printf("Sending EOI for IRQ%d\n", irq_num);
    uint32_t signal = irq_num | EOI;
    // Master interrupt
    if(irq_num < 8) outb(signal, MASTER_8259_PORT_2);
    // Slave interrupt
    else outb(signal, SLAVE_8259_PORT_2);
}
