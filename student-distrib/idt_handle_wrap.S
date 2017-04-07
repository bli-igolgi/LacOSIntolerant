/*
 * The assembly wrapper functions for ISR invocation
 */
#define ASM	1

.text

/*	Exporting exception, interrupt, and system call handlers to be used by idt_handle.c */
.globl _divide_by_zero, _reserved, _non_maskable_interrupt, _breakpoint, _overflow                         # vector 0x00-0x04
.globl _bound_range_exceeded, _undefined_opcode, _no_math_coprocessor, _double_fault, _coprocessor_overrun # 0x05 - 0x09
.globl _invalid_tss, _segment_not_present, _stack_segment_fault, _general_protection, _page_fault          # 0x0A - 0x0E
.globl _intel_reserved, _floating_point_error, _alignment_check, _machine_check, _floating_point_except    # 0x0F - 0x13

.globl _keyboard_intr, _rtc_intr, _mouse_intr

.globl _system_call                     # 0x80

.align 4




/*
From discussion on system calls

Open
    Find the file in the file system and assign an unused file descriptor
    File descriptors need to be set up according to the file type
    Never check file names in open, only care about file types
    Map read, write, and other functions
Close
    Close the file descriptor passed in (set it to be available)
    Check for invalid descriptors
Read, write
    Call the corresponding read or write function

*/

/*
From discussion on paging and program loader

Program always goes between 128-132MB in virtual memory
Physical memory starts at 8MB + (process number * 4MB)
    Process number starts from 0
Flush TLB after swapping page
Must be able to support 2 processes for checkpoint 3 (8-12 and 12-16MB)

Program loader
    Checks ELF magic constant
    Copy file contents to the correct location (don't use memcpy, because file might be larger than 8kB, and then will go to next PCB)
    Find the first instruction's address

PCB
    Each process has its own PCB
        Call system.exe in kernel.c to test system call
    Each PCB starts at the top of a 8kB block inside the kernel
        First PCB starts at  8MB - 1(8kB)
        Second PCB starts at 8MB - 2(8kB)
            Second kernel stack starts at 1st PCB address - 4B
        etc.
    Switch the kernel stack when switching PCB
    What you need in a PCB:
        file descriptor array
        whatever you think is necessary for running/stopping a program and should be process specific

To execute file - look at picture on piazza as a reference
    First check arguments (everything before first space is argument, everything after is arguments, must trim spaces)
    Save arguments
    Check file validity (whether it is there, if it is executable)
    Set up paging - Use 4MB page to map to 128-132MB
    Flush the TLB right away
    Load the file to memory
    Start setting up PCB
    Prepare the context switch (change certain registers)
    Push IRET context onto stack (should be 5 different 32 bit values, may need to extend them)
        1. Return address (the first instruction address)
        2. Other values to help do context switch
    Call IRET (will take values, do magic, and go somewhere else)
        Should go to first process we are trying to run (shell)
        We should be able to type something to the shell now
            Everything is the same for the typed command to set up, except paging
            Make sure to not save page for process run from shell in the same place, otherwise won't be able to return to shell

    Halt
        Restore values set up in PCB, depends on how PCB was set up initially (be careful)
        Jump back to execute, stopped right after the IRET instruction when setting up the first PCB

    esp0 is the stack pointer for kernel (privilege level 0)

*/




/* ========= SYSTEM CALL ========= */
/* NOTE: system call is currently rudimentary. improvements will be made once call process & implementation is clearly laid out */
# Description: wrapper to service system calls: see .c file on sys_call
#	   Inputs:	eax - cmd
#				ebx - arg1
#				ecx - arg2
#				edx - arg3
#	  Outputs: returns 0 or -1 in eax, in accordance with sys_call (the C handler)
#	 Clobbers: eax, ecx, edx (ebx preserved after call by cdecl)
_system_call:
    
    call    sys_call
    iret

    /*
    // Old code
    # Push 4 call params from right (arg3) to left (cmd)
    pushl   %edx
    pushl   %ecx
    pushl   %ebx
    pushl   %eax
    call    sys_call
	# Remove arguments, and return value is stored in eax
    leal    16(%esp), %esp
    iret
    */
    
/* ========= INTERRUPTS ========= */

# Description: wrapper to service KEYBOARD interrupts
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none; all registers are explicitly preserved
_keyboard_intr:
    pushal
    call keyboard_interrupt
    popal
    iret

# Description: wrapper to service RTC interrupts
#      Inputs: none
#     Outputs: none
#    Clobbers: none; all registers are explicitly preserved
_rtc_intr:
    pushal
    call rtc_interrupt
    popal
    iret

# Description: wrapper to service mouse interrupts
#      Inputs: none
#     Outputs: none
#    Clobbers: none; all registers are explicitly preserved
_mouse_intr:
    pushal
    call mouse_interrupt
    popal
    iret

/* ========= EXCEPTIONS ========= */
/*
 * All of these call their respective C exception handler with interrupts masked.
 * No registers are explicitly preserved under current [tag] 5.1.7 code, as it's not necessary.
 */

# Description: exception wrapper for vector 0x00
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_divide_by_zero:
    call    div_zero_fault
    iret

# Description: exception wrapper for vector 0x01
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_reserved:
    call    reserved_fault
    iret

# Description: exception wrapper for vector 0x02
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_non_maskable_interrupt:
    call    nmi_intr
    iret

# Description: exception wrapper for vector 0x03
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_breakpoint:
    call    breakpoint_trap
    iret

# Description: exception wrapper for vector 0x04
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_overflow:
    call    overflow_trap
    iret
    
# Description: exception wrapper for vector 0x05
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_bound_range_exceeded:
    call    bound_range_fault
    iret
    
# Description: exception wrapper for vector 0x06
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_undefined_opcode:
    call    invalid_opcode_fault
    iret
    
# Description: exception wrapper for vector 0x07
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_no_math_coprocessor:
    call    device_na_fault
    iret
    
# Description: exception wrapper for vector 0x08
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_double_fault:
    call    double_fault_abort
    iret
    
# Description: exception wrapper for vector 0x09
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_coprocessor_overrun:
    call    seg_overrun_fault
    iret
    
# Description: exception wrapper for vector 0x0A
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_invalid_tss:
    call    tss_fault
    iret
    
# Description: exception wrapper for vector 0x0B
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_segment_not_present:
    call    seg_np_fault
    iret

# Description: exception wrapper for vector 0x0C
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_stack_segment_fault:
    call    ss_fault
    iret
    
# Description: exception wrapper for vector 0x0D
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_general_protection:
    call    gen_pro_fault
    iret

# Description: exception wrapper for vector 0x0E
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_page_fault:
    call    page_fault
    iret
    
# Description: exception wrapper for vector 0x0F
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_intel_reserved:
    call    dne_entry
    iret
    
# Description: exception wrapper for vector 0x10
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_floating_point_error:
    call    fpu_math_fault
    iret
    
# Description: exception wrapper for vector 0x11
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_alignment_check:
    call    align_fault
    iret
    
# Description: exception wrapper for vector 0x12
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_machine_check:
    call    machine_chk_abort
    iret
    
# Description: exception wrapper for vector 0x13
#	   Inputs: none
#	  Outputs: none
#	 Clobbers: none
_floating_point_except:
    call    simd_fpe_fault
    iret
