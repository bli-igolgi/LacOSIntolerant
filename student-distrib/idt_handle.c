#include "idt_handle.h"

//  function pointer array for exception handlers (wrapper)
void (*idt_ptr[20]) = {
    _divide_by_zero, _reserved, _non_maskable_interrupt, _breakpoint, _overflow,				            //0x00 - 0x04
    _bound_range_exceeded, _undefined_opcode, _no_math_coprocessor, _double_fault, _coprocessor_overrun,    //0x05 - 0x09
    _invalid_tss, _segment_not_present, _stack_segment_fault, _general_protection, _page_fault,			    //0x0A - 0x0E
    _intel_reserved, _floating_point_error, _alignment_check, _machine_check, _floating_point_except        //0x0F - 0x13
    };
/*
    function header here 
*/
void idt_init(){
    idt_desc_t idt_entry;
    int i;
    
    //  make entry for trap gates
    SET_TRAP_GATE(idt_entry);
    
    //  fill in idt table entries #0 - #19 as trap gates
    for(i = 0; i < 20; i++){
        SET_IDT_ENTRY(idt_entry, idt_ptr[i]);
        idt[i] = idt_entry;
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
void overflow_trap() {
    printf("Exception 0x04: Overflow detected. It's too big.");
    while(1);
    return;
}

/* function header here */
void bound_range_fault() {
    printf("Exception 0x05: Bounds exceeded. Try a tigher fit.");
    while(1);
    return;
}

/* function header here */
void invalid_opcode_fault() {
    printf("Exception 0x06: Invalid opcode used. This is x86, bruh...");
    while(1);
    return;
}

/* function header here */
void device_na_fault() {
    printf("Exception 0x07: Device not available. You gonna wait today.");
    while(1);
    return;
}

/* function header here */
void double_fault_abort() {
    printf("Exception 0x08: Double fault abort! RIP, GG.");
    while(1);
    return;
}

/* function header here */
void seg_overrun_fault() {
    printf("Exception 0x09: Coprocessor Segment Overrun (reserved).");
    while(1);
    return;
}

/* function header here */
void tss_fault() {
    printf("Exception 0x0A: Invalid task state segment. No task switching for you, ha.");
    while(1);
    return;
}

/* function header here */
void seg_np_fault() {
	printf("Exception 0x0B: Segment not present. That's the wrong segment!");
    while(1);
    return;
}

/* function header here */
void ss_fault() {
	printf("Exception 0x0C: Stack segment fault. Have fun debugging.");
    while(1);
    return;
}

/* function header here */
void gen_pro_fault() {
	printf("Exception 0x0D: General protection fault. You're violating me.");
    while(1);
    return;
}

/* function header here */
void page_fault() {
	printf("Exception 0x0E: Page fault. Cache me osside, how bow dat?");
    while(1);
    return;
}

/* function header here */
void dne_entry() {
	printf("Exception 0x0F: Intel reserved. Why are you even using this?");
    while(1);
    return;
}

/* function header here */
void fpu_math_fault() {
	printf("Exception 0x10: x87 FPU Floating-Point Error.");
    while(1);
    return;
}

/* function header here */
void align_fault() {
	printf("Exception 0x11: Alignment check fault. Guess your brain needs re-alignment.");
    while(1);
    return;
}

/* function header here */
void machine_chk_abort() {
	printf("Exception 0x12: Machine check abort! Another RIP, GG.");
    while(1);
    return;
}

/* function header here */
void simd_fpe_fault() {
	printf("Exception 0x13: Floating point exception.");
    while(1);
    return;
}
