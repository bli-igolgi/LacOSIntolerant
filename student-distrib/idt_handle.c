#include "idt_handle.h"

/*
    function header here 
*/
void idt_init(){
    idt_desc_t except_entry;
    int i;                  // Loop index
    
    SET_TRAP_GATE(except_entry);
    
    for(i = 0; i < 2; i++) {
        SET_IDT_ENTRY(except_entry, _idt_handler_table + i*32);
        idt[i] = except_entry;
    }

    return;
}



/* ========== C Interrupt Handlers ========== */
/* The following functions are all the C handling functions for interrupts */


/*
    function header here 
*/
void div_zero_fault() {
    printf("Exception 0x00: Divide-by-zero fault occurred. Do you even math?");
    while(1);
    return;
}

/*
    function header here 
*/
void debug_trap() {
    printf("Exception 0x01: Debug exception thrown. Waiting indefinitely...");
    while(1);
    return;
}
