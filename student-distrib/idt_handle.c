#include "idt_handle.h"

/*
    function header here 
*/
void idt_init(){
    idt_desc_t except_entry;
    int i;                  // Loop index
    
    SET_TRAP_GATE(except_entry);
    
    for(i = 0; i < 4; i++) {
        SET_IDT_ENTRY(except_entry, _idt_handler_table + i*6);
        idt[i] = except_entry;
    }

    return;
}


/* ========== C Interrupt Handlers ========== */
/* The following functions are all the C handling functions for interrupts */


/* function header here */
void div_zero_fault() {
    printf("Exception 0x00: Divide-by-zero fault occurred. Do you even math?");
    while(1);
    return;
}

/* function header here */
void reserved_fault() {
    printf("Exception 0x01: RESERVED. For intel used only...");
    while(1);
    return;
}

/* function header here */
void nmi_intr() {
    printf("Exception 0x02: Non-maskable interrupt. Hardware wants attention.");
    while(1);
    return;
}

/* function header here */
void breakpoint_trap() {
    printf("Exception 0x03: Breakpoint thrown. How long is this going to take?");
    while(1);
    return;
}

/* function header here */
void seg_np_fault() {
	printf("Exception 0x0B: Seg");
    while(1);
    return;
}

/* function header here */
void ss_fault() {
	printf("Exception 0x0C: Segmentation Fault. HF debugging.");
    while(1);
    return;
}