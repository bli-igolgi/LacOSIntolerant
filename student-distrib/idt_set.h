#ifndef IDT_SET_H
#define IDT_SET_H

#include "x86_desc.h"
#include "lib.h"
#include "int_wrap.h"

extern void set_idt_handlers(void);

extern void divide_by_zero(void);

#endif
