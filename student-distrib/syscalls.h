#ifndef SYS_CALLS_H
#define SYS_CALLS_H

/* Constants used by the execute system call */
#define FILE_H_SIZE      30
#define PROGRAM_1_PHYS   0x800000       // 8MB
#define PROGRAM_2_PHYS   0xC00000       // 12MB
#define PROGRAM_VIRT     0x8000000      // 128MB
#define PROGRAM_VIRT_OFF 0x48000        // Where to start copying the program image within the virtual space
#define PAGE_4MB         0x400000         // size of a 4MB page

/* File type constants
#define RTC_DEV		0
#define DIR_TYPE	1
#define REG_FILE	2 */

#ifndef ASM

#include "lib.h"
#include "paging.h"
#include "pcb.h"
#include "file_system.h"
#include "x86_desc.h"

extern f_ops_table rtc_jt;
extern f_ops_table dir_jt;
extern f_ops_table regf_jt;

/* The actual system calls */
int32_t sys_halt(uint8_t status);
int32_t sys_execute(const uint8_t *command);
int32_t sys_read(int32_t fd, void *buf, int32_t nbytes);
int32_t sys_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t sys_open(const uint8_t *filename);
int32_t sys_close(int32_t fd);
int32_t sys_getargs(uint8_t *buf, int32_t nbytes);
int32_t sys_vidmap(uint8_t **screen_start);
int32_t sys_set_handler(int32_t signum, void *handler_address);
int32_t sys_sigreturn(void);

extern uint32_t except_raised;

#define flush_tlb()                     \
do {                                    \
    asm volatile(                       \
        "movl %%cr3,%%eax;"             \
        "movl %%eax,%%cr3;"             \
        :                               \
        :                               \
        : "%eax"                        \
    );                                  \
} while(0)

#endif /* ASM */

#endif /* SYS_CALLS_H */
