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
#define SET_TRAP_GATE(str)          \
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

extern uint32_t _idt_handler_table;

/*	exception handlers #0 - #19 defined by i386	*/


#endif /* ASM */

#endif /* IDT_HANDLE_H */
