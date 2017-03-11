#ifndef IDT_HANDLE_H
#define IDT_HANDLE_H

#ifndef ASM

#include "x86_desc.h"
#include "lib.h"

//  designate idt_desc_t struct as trap gate with default values
#define SET_TRAP_GATE(str)          \
do {                                \
    str.seg_selector = KERNEL_CS;   \
    str.reserved3 = 1;              \
    str.reserved2 = 1;              \
    str.reserved1 = 1;              \
    str.size = 1;                   \
    str.reserved0 = 0;              \
    str.dpl = 0;                    \
    str.present = 1;                \
} while(0)
	
//  designate idt_desc_t struct as interrupt gate with default values
#define SET_INTR_GATE(str)          \
do {                                \
    str.seg_selector = KERNEL_CS;   \
    str.reserved3 = 0;              \
    str.reserved2 = 1;              \
    str.reserved1 = 1;              \
    str.size = 1;                   \
    str.reserved0 = 0;              \
    str.dpl = 0;                    \
    str.present = 1;                \
} while(0)

//  initializes interrupt descriptor table
extern void idt_init(void);

/*	exception handlers #0 - #19 defined by i386	*/
void _divide_by_zero(void);
void _reserved(void);
void _non_maskable_interrupt(void);
void _breakpoint(void);
void _overflow(void);
void _bound_range_exceeded(void);
void _undefined_opcode(void);
void _no_math_coprocessor(void);
void _double_fault(void);
void _coprocessor_overrun(void);
void _invalid_tss(void);
void _segment_not_present(void);
void _stack_segment_fault(void);
void _general_protection(void);
void _page_fault(void);
void _intel_reserved(void);
void _floating_point_error(void);
void _alignment_check(void);
void _machine_check(void);
void _floating_point_except(void);


#endif /* ASM */

#endif /* IDT_HANDLE_H */
