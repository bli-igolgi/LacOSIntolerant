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

.globl _syscall                     # 0x80

.align 4

/*
MP3.4
    Should be able to run all executables except sigtest

*/

/* ========= SYSTEM CALL ========= */

# Description: wrapper to service system calls
#	   Inputs:	eax - system call number
#				ebx - arg1
#				ecx - arg2
#				edx - arg3
#	  Outputs: -1 for failure, otherwise whatever is returned by system call C function
#	 Clobbers: eax
_syscall:
    /* Check that command argument is valid, between 1 and 10 */
    cmpl    $0,%eax
    jle      _sys_call_fail
    cmpl    $10,%eax
    jg      _sys_call_fail

    # Save the unused registers
    pushl   %edi
    pushl   %esi
    # Push the arguments onto the stack
    pushl   %edx
    pushl   %ecx
    pushl   %ebx

    # Call the C system call corresponding to the sys call number
	subl	$1, %eax						# shift cmd from index 1 to index 0
    call    *_sys_call_jmp_tab(, %eax, 4)

    # Restore the register values
    popl    %ebx
    popl    %ecx
    popl    %edx
    popl    %esi
    popl    %edi

    jmp     _sys_call_done

_sys_call_fail:
    movl $-1, %eax

_sys_call_done:
    iret

_sys_call_jmp_tab:
    .long sys_halt, sys_execute, sys_read, sys_write
    .long sys_open, sys_close, sys_getargs
    .long sys_vidmap, sys_set_handler, sys_sigreturn
    
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
