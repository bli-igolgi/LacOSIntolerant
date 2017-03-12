#include "idt_handle.h"

//  function pointer array for exception handlers (wrapper)
void (*except_ptr[20]) = {
    _divide_by_zero, _reserved, _non_maskable_interrupt, _breakpoint, _overflow,                            // 0x00 - 0x04
    _bound_range_exceeded, _undefined_opcode, _no_math_coprocessor, _double_fault, _coprocessor_overrun,    // 0x05 - 0x09
    _invalid_tss, _segment_not_present, _stack_segment_fault, _general_protection, _page_fault,             // 0x0A - 0x0E
    _intel_reserved, _floating_point_error, _alignment_check, _machine_check, _floating_point_except        // 0x0F - 0x13
};

void (*intr_ptr[2]) = {
    keyboard_interrupt, rtc_interrupt
};
    
/*
    function header here 
*/
void idt_init(){
    idt_desc_t idt_entry;
    idt_desc_t empty_entry;
    empty_entry.present = 0;
    int i;
    
    //  fill in idt table entries #0 - #19 as trap gates
    SET_TRAP_GATE(idt_entry);
    for(i = 0; i < USED_EXCEPTIONS; i++){
        if(i == 15)
            idt[i] = empty_entry;       // intel reserved - not used
        
        SET_IDT_ENTRY(idt_entry, except_ptr[i]);
        idt[i] = idt_entry;
    }
    
    // entries #20 - #31 are empty : intel reserved
    for(i = USED_EXCEPTIONS; i < NUM_EXCEPTIONS; i++)
        idt[i] = empty_entry;
    
    // fill in idt table interrupt entries here
    SET_INTR_GATE(idt_entry);

    // Set the keyboard entry
    SET_IDT_ENTRY(idt_entry, intr_ptr[0]);
    idt[0x21] = idt_entry;

    // Set the rtc entry
    SET_IDT_ENTRY(idt_entry, intr_ptr[1]);
    idt[0x28] = idt_entry;
    
    //  fill in idt table entry #128 as system call
    SET_TRAP_GATE(idt_entry);
    idt_entry.dpl = 3;
    // SET_IDT_ENTRY(idt_entry, _system_call);
    idt[SYS_VECTOR] = idt_entry;
    
    // define rest of the entries as empty
    
    
    return;
}

/* ========== Exception Handlers ========== */
/* The following functions are all the C handling functions for interrupts */

void div_zero_fault() {
    printf("Exception 0x00: Divide-by-zero fault occurred. Do you even math?");
    while(1);
    return;
}

void reserved_fault() {
    printf("Exception 0x01: RESERVED. For intel used only...");
    while(1);
    return;
}

void nmi_intr() {
    printf("Exception 0x02: Non-maskable interrupt. Hardware wants attention.");
    while(1);
    return;
}

void breakpoint_trap() {
    printf("Exception 0x03: Breakpoint thrown. How long is this going to take?");
    while(1);
    return;
}

void overflow_trap() {
    printf("Exception 0x04: Overflow detected. It's too big.");
    while(1);
    return;
}

void bound_range_fault() {
    printf("Exception 0x05: Bounds exceeded. Try a tigher fit.");
    while(1);
    return;
}

void invalid_opcode_fault() {
    printf("Exception 0x06: Invalid opcode used. This is x86, bruh...");
    while(1);
    return;
}

void device_na_fault() {
    printf("Exception 0x07: Device not available. You gonna wait today.");
    while(1);
    return;
}

void double_fault_abort() {
    printf("Exception 0x08: Double fault abort! RIP, GG.");
    while(1);
    return;
}

void seg_overrun_fault() {
    printf("Exception 0x09: Coprocessor Segment Overrun (reserved).");
    while(1);
    return;
}

void tss_fault() {
    printf("Exception 0x0A: Invalid task state segment. No task switching for you, ha.");
    while(1);
    return;
}

void seg_np_fault() {
    printf("Exception 0x0B: Segment not present. That's the wrong segment!");
    while(1);
    return;
}

void ss_fault() {
    printf("Exception 0x0C: Stack segment fault. Have fun debugging.");
    while(1);
    return;
}

void gen_pro_fault() {
    printf("Exception 0x0D: General protection fault. You're violating me.");
    while(1);
    return;
}

void page_fault() {
    printf("Exception 0x0E: Page fault. Cache me osside, how bow dat?");
    while(1);
    return;
}

void dne_entry() {
    printf("Exception 0x0F: Intel reserved. Why are you even using this?");
    while(1);
    return;
}

void fpu_math_fault() {
    printf("Exception 0x10: x87 FPU Floating-Point Error.");
    while(1);
    return;
}

void align_fault() {
    printf("Exception 0x11: Alignment check fault. Guess your brain needs re-alignment.");
    while(1);
    return;
}

void machine_chk_abort() {
    printf("Exception 0x12: Machine check abort! Another RIP, GG.");
    while(1);
    return;
}

void simd_fpe_fault() {
    printf("Exception 0x13: Floating point exception.");
    while(1);
    return;
}
