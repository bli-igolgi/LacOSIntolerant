/*
 * Sets up the IDT, filling the table with the respective handling functions
 */

#include "idt_handle.h"

// Function pointer array for all 20 default exception handlers (wrapper)
void (*except_ptr[20]) = {
    _divide_by_zero, _reserved, _non_maskable_interrupt, _breakpoint, _overflow,
    _bound_range_exceeded, _undefined_opcode, _no_math_coprocessor, _double_fault, _coprocessor_overrun,
    _invalid_tss, _segment_not_present, _stack_segment_fault, _general_protection, _page_fault,
    _intel_reserved, _floating_point_error, _alignment_check, _machine_check, _floating_point_except
};

// Function pointer array for all 16 PIC interrupt lines handlers (wrapper)
void (*intr_ptr[16]) = {
    _pit_intr, _keyboard_intr, 0, 0, 0, 0, 0, 0,       // master PIC ([2] is slave, is not used)
    _rtc_intr, 0, 0, 0, _mouse_intr, 0, 0, 0           // slave PIC
};

/*
 * void idt_init(void);
 *   Inputs: none
 *   Return Value: none
 *   Function: Puts the exceptions, interrupts, and system call
 *              vectors inside the IDT, with their associated wrappers
 */
void idt_init() {
    idt_desc_t idt_entry, empty_entry;
    empty_entry.present = 0;
    int i;
    
    // Fill in idt table entries #0 - #19 as non-interruptible gates
    SET_INTR_GATE(idt_entry);
    
    for(i = 0; i < USED_EXCEPTIONS; i++) {
        SET_IDT_ENTRY(idt_entry, except_ptr[i]);
        idt[i] = idt_entry;
    }
    
    /* entries #20 - #31 are empty : intel reserved
       entries #32 - #255 are user-defined : devices will request them as needed */
    for(i = USED_EXCEPTIONS; i < NUM_VEC; i++)
        idt[i] = empty_entry;

    
    // defining entries #32 - #47 map to master & slave PICs interrupts
    for(i = 0; i < PIC_INTRS; i++)
        if(intr_ptr[i]) {
            //  fill in VALID (only) intr handlers from PIC lines starting at IDT index #32 / 0x20
            SET_IDT_ENTRY(idt_entry, intr_ptr[i]);
            idt[i + 32] = idt_entry;
        }
    
    // Fill in idt table below this code as interruptible gates
    SET_TRAP_GATE(idt_entry);
    
    // Fill in idt table entry #128 / 0x80 with system call handler (as trap)
    idt_entry.dpl = 3;              //system calls are avail to user programs at DPL 3
    SET_IDT_ENTRY(idt_entry, _syscall);
    idt[128] = idt_entry;
    
    // Load the interrupt table
    lidt(idt_desc_ptr);
}

/* ========== Exception Handlers ========== */

/*
 *   Inputs: none
 *   Return Value: none
 *   Function: Handles exception invoked by corresponding IDT vector number,
 *              prints offending error msg, stalls system indefinitely
 */
void div_zero_fault() { PRINT_MESSAGE("Divide-by-zero fault occurred. Do you even math?\n"); }
void reserved_fault() { PRINT_MESSAGE("RESERVED. You got an exception for intel used only...\n"); }
void nmi_intr() { PRINT_MESSAGE("Non-maskable interrupt triggered. Hardware wants attention bad.\n"); }
void breakpoint_trap() { PRINT_MESSAGE("Breakpoint thrown. How long is this going to take?\n"); }
void overflow_trap() { PRINT_MESSAGE("Overflow detected. It's too big.\n"); }
void bound_range_fault() { PRINT_MESSAGE("Bounds exceeded. Try a tigher fit.\n"); }
void invalid_opcode_fault() { PRINT_MESSAGE("Invalid opcode used. This is x86, bruh...\n"); }
void device_na_fault() { PRINT_MESSAGE("Device not available fault. You gonna wait today.\n"); }
void double_fault_abort() { PRINT_MESSAGE("Double fault, abort! RIP, GG.\n"); }
void seg_overrun_fault() { PRINT_MESSAGE("Coprocessor Segment Overrun (reserved). Really shouldn't be getting this...\n"); }
void tss_fault() { PRINT_MESSAGE("Invalid task state segment. No context switching for you, ha.\n"); }
void seg_np_fault() { PRINT_MESSAGE("Segment not present. Is your brain segment present?\n"); }
void ss_fault() { PRINT_MESSAGE("Stack segment fault. Have fun debugging.\n"); }
void gen_pro_fault() { PRINT_MESSAGE("General protection fault. You're violating me.\n"); }
void dne_entry() { PRINT_MESSAGE("Intel reserved. Why are you even getting this?!\n"); }
void fpu_math_fault() { PRINT_MESSAGE("x87 FPU Floating-Point Error. No one actually knows...\n"); }
void align_fault() { PRINT_MESSAGE("Alignment check faulted. Guess your brain also needs re-alignment.\n"); }
void machine_chk_abort() { PRINT_MESSAGE("Machine check failed, abort! Another RIP, GG.\n"); }
void simd_fpe_fault() { PRINT_MESSAGE("Floating point exception. IDEK, but GL.\n"); }

/* Does the above, as well as prints the error found in register CR2 */
void page_fault() {
    uint32_t cr2;

    // Get the error stored in cr2
    asm volatile (
        "movl %%cr2, %%eax;"
        "movl %%eax, %0;"
        : "=m"(cr2)
        :
        : "%eax"
    );

    printf("Page fault encountered with error %x\n", cr2);
    except_raised = 1;
    sys_halt(0);
    return;
}
