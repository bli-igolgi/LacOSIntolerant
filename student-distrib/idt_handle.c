#include "idt_set.h"

/*
    function header here 
*/
void idt_init(){
    idt_desc_t idt_entry;
    
    SET_TRAP_GATE(idt_entry);
    
    SET_IDT_ENTRY(idt_entry, _divide_by_zero);

    idt[0] = idt_entry;
    return;
}

/*
    function header here 
*/
void div_zero_fault() {
    printf("Divide-by-zero fault occurred. Do you even math?");
    while(1);
    return;
}

/*
    function header here 
*/
void debug_trap() {
    printf("Debug exception thrown. Waiting indefinitely...");
    while(1);
    return;
}