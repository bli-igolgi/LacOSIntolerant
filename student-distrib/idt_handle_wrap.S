#define ASM     1

.text

.globl _idt_handler_table

_divide_by_zero:
    call div_zero_fault
    iret

_debug_exception:
    call debug_trap
    iret

// Interrupt?????????????????????????????????????????
_non_maskable_interrupt:
    call nmi_trap
    iret

_breakpoint:
    call  breakpoint_trap
    iret




_idt_handler_table:
    .long _divide_by_zero, _debug_exception, _non_maskable_interrupt, _breakpoint
