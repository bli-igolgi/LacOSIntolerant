#define ASM     1

.text

.globl _divide_by_zero
.globl _debug_exception

_divide_by_zero:
    call div_zero_fault
    iret

_debug_exception:
    call debug_trap
    iret
