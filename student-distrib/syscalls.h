#ifndef SYS_CALLS_H
#define SYS_CALLS_H

/* The vector IDT numbers for the system calls */
#define SYS_HALT    1
#define SYS_EXECUTE 2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_GETARGS 7
#define SYS_VIDMAP  8
#define SYS_SET_HANDLER  9
#define SYS_SIGRETURN  10

#ifndef ASM

/* The actual system calls */
int32_t __ece391_halt(uint8_t status);
int32_t __ece391_execute(const uint8_t *command);
int32_t __ece391_read(int32_t fd, void *buf, int32_t nbytes);
int32_t __ece391_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t __ece391_open(const uint8_t *filename);
int32_t __ece391_lose(int32_t fd);
int32_t __ece391_getargs(uint8_t *buf, int32_t nbytes);
int32_t __ece391_vidmap(uint8_t **screen_start);
int32_t __ece391_set_handler(int32_t signum, void *handler_address);
int32_t __ece391_sigreturn(void);




#endif /* ASM */

#endif /* SYS_CALLS_H */
