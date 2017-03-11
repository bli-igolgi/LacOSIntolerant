#define ASM 1
#include "int_wrap.h"

.text

.globl asm_divide_by_zero

.align 4

asm_divide_by_zero:
    call asm_divide_by_zero
    call divide_by_zero
    iret


// int_wrappers
    // .long asm_divide_by_zero
