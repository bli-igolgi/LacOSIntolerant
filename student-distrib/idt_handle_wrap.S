/*
 * The assembly wrapper functions for ISR invocation
 */
#define ASM	1

// Macro to define all of the system interrupts
.macro InterruptWrap asm_wrap, c_wrap
    .GLOBL \asm_wrap          
    \asm_wrap:  
        pushal
        call \c_wrap
        popal
        iret                  
.endm

// Macro to define all of the system exceptions
.macro ExceptionWrap asm_wrap, c_wrap
    .GLOBL \asm_wrap          
    \asm_wrap:                
        call    \c_wrap       
        iret                     
.endm

.text

.globl _syscall                     # 0x80

.align 4

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

InterruptWrap _keyboard_intr, keyboard_interrupt
InterruptWrap _rtc_intr, rtc_interrupt
InterruptWrap _mouse_intr, mouse_interrupt


ExceptionWrap _divide_by_zero, div_zero_fault
ExceptionWrap _reserved, reserved_fault
ExceptionWrap _non_maskable_interrupt, nmi_intr
ExceptionWrap _breakpoint, breakpoint_trap
ExceptionWrap _overflow, overflow_trap

ExceptionWrap _bound_range_exceeded, bound_range_fault
ExceptionWrap _undefined_opcode, invalid_opcode_fault
ExceptionWrap _no_math_coprocessor, device_na_fault
ExceptionWrap _double_fault, double_fault_abort
ExceptionWrap _coprocessor_overrun, seg_overrun_fault

ExceptionWrap _invalid_tss, tss_fault
ExceptionWrap _segment_not_present, seg_np_fault
ExceptionWrap _stack_segment_fault, ss_fault
ExceptionWrap _general_protection, gen_pro_fault
ExceptionWrap _page_fault, page_fault

ExceptionWrap _intel_reserved, dne_entry
ExceptionWrap _floating_point_error, fpu_math_fault
ExceptionWrap _alignment_check, align_fault
ExceptionWrap _machine_check, machine_chk_abort
ExceptionWrap  _floating_point_except, simd_fpe_fault
