#define ASM     1

.text

.globl _divide_by_zero
.globl _debug_exception
.globl _idt_handler_table

_divide_by_zero:
    call div_zero_fault
    iret

_debug_exception:
    call debug_trap
    iret

_idt_handler_table:
    .long _divide_by_zero, _debug_exception
