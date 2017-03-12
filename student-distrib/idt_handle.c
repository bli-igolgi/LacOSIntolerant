#include "idt_handle.h"

//  function pointer array for all 20 default exception handlers (wrapper)
void (*except_ptr[20]) = {
    _divide_by_zero, _reserved, _non_maskable_interrupt, _breakpoint, _overflow,
    _bound_range_exceeded, _undefined_opcode, _no_math_coprocessor, _double_fault, _coprocessor_overrun,
    _invalid_tss, _segment_not_present, _stack_segment_fault, _general_protection, _page_fault,
    _intel_reserved, _floating_point_error, _alignment_check, _machine_check, _floating_point_except
};

//	function pointer array for all 16 PIC interrupt lines handlers (wrapper)
void (*intr_ptr[16]) = {
    0, _keyboard_intr, 0, 0, 0, 0, 0, 0,        // master PIC
	_rtc_intr, 0, 0, 0, 0, 0, 0, 0              // slave PIC
};
    
/*
    function header here 
*/
void idt_init(){
    idt_desc_t idt_entry, empty_entry;
    empty_entry.present = 0;
    int i;
    
    // Fill in idt table entries #0 - #19 as trap gates
    SET_TRAP_GATE(idt_entry);
    for(i = 0; i < USED_EXCEPTIONS; i++) {
        SET_IDT_ENTRY(idt_entry, except_ptr[i]);
        idt[i] = idt_entry;
    }
    
    /* entries #20 - #31 are empty : intel reserved
       entries #32 - #256 are user-defined : devices will request them as needed */
    for(i = USED_EXCEPTIONS; i < NUM_VEC; i++)
        idt[i] = empty_entry;

	
	// Entries 0x20 - 0x2F map to master & slave PICs interrupts
	SET_INTR_GATE(idt_entry);
	for(i = 0; i <= 0x0F; i++)
		if(intr_ptr[i]) {
			SET_IDT_ENTRY(idt_entry, intr_ptr[i]);
            // Add 0x20 to offset to the interrupt entry section
			idt[i + 0x20] = idt_entry;
		}
    
    // Fill in idt table entry 0x80 with system call handler (as trap)
    SET_TRAP_GATE(idt_entry);
    idt_entry.dpl = 3;
    SET_IDT_ENTRY(idt_entry, _system_call);
    // 0x80 is the system call entry in the IDT
    idt[0x80] = idt_entry;
    
    return;
}

/* ========== System Call Handler ========== */
int32_t syscall(int32_t cmd, int32_t arg1, int32_t arg2, int32_t arg3) {
    printf("The system call you've called is not available right now. Please try again later.\n");
    return 0;
}

/* ========== Exception Handlers ========== */
/* The following functions are all the C handling functions for exceptions */

void div_zero_fault() {
    printf("Exception 0x00: Divide-by-zero fault occurred. Do you even math?\n");
    while(1);
    return;
}

void reserved_fault() {
    printf("Exception 0x01: RESERVED. For intel used only...\n");
    while(1);
    return;
}

void nmi_intr() {
    printf("Exception 0x02: Non-maskable interrupt. Hardware wants attention.\n");
    while(1);
    return;
}

void breakpoint_trap() {
    printf("Exception 0x03: Breakpoint thrown. How long is this going to take?\n");
    while(1);
    return;
}

void overflow_trap() {
    printf("Exception 0x04: Overflow detected. It's too big.\n");
    while(1);
    return;
}

void bound_range_fault() {
    printf("Exception 0x05: Bounds exceeded. Try a tigher fit.\n");
    while(1);
    return;
}

void invalid_opcode_fault() {
    printf("Exception 0x06: Invalid opcode used. This is x86, bruh...\n");
    while(1);
    return;
}

void device_na_fault() {
    printf("Exception 0x07: Device not available. You gonna wait today.\n");
    while(1);
    return;
}

void double_fault_abort() {
    printf("Exception 0x08: Double fault abort! RIP, GG.\n");
    while(1);
    return;
}

void seg_overrun_fault() {
    printf("Exception 0x09: Coprocessor Segment Overrun (reserved).\n");
    while(1);
    return;
}

void tss_fault() {
    printf("Exception 0x0A: Invalid task state segment. No task switching for you, ha.\n");
    while(1);
    return;
}

void seg_np_fault() {
    printf("Exception 0x0B: Segment not present. That's the wrong segment!\n");
    while(1);
    return;
}

void ss_fault() {
    printf("Exception 0x0C: Stack segment fault. Have fun debugging.\n");
    while(1);
    return;
}

void gen_pro_fault() {
    printf("Exception 0x0D: General protection fault. You're violating me.\n");
    while(1);
    return;
}

void page_fault() {
    printf("Exception 0x0E: Page fault. Cache me osside, how bow dat?\n");
    while(1);
    return;
}

void dne_entry() {
    printf("Exception 0x0F: Intel reserved. Why are you even using this?\n");
    while(1);
    return;
}

void fpu_math_fault() {
    printf("Exception 0x10: x87 FPU Floating-Point Error.\n");
    while(1);
    return;
}

void align_fault() {
    printf("Exception 0x11: Alignment check fault. Guess your brain needs re-alignment.\n");
    while(1);
    return;
}

void machine_chk_abort() {
    printf("Exception 0x12: Machine check abort! Another RIP, GG.\n");
    while(1);
    return;
}

void simd_fpe_fault() {
    printf("Exception 0x13: Floating point exception.\n");
    while(1);
    return;
}
