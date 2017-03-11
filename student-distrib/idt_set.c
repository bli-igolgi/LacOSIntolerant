#include "idt_set.h"


idt_desc_t exception_entry;

void set_idt_handlers() {
    SET_IDT_ENTRY(exception_entry, divide_by_zero);
    exception_entry.seg_selector = KERNEL_CS;
    exception_entry.dpl = 0;
    exception_entry.present = 1;
    exception_entry.size = 1;

    exception_entry.reserved0 = 0;
    exception_entry.reserved1 = 1;
    exception_entry.reserved2 = 1;
    exception_entry.reserved3 = 1;

    idt[0] = exception_entry;
    return;
}

void divide_by_zero() {
    printf("divide_by_zero %d", 0);
    while(1);
    return;
}