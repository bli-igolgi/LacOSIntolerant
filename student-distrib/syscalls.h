#ifndef SYS_CALLS_H
#define SYS_CALLS_H

#ifndef ASM

int32_t halt(uint8_t status);
int32_t execute(const uint8_t *command);
int32_t read(int32_t fd, void *buf, int32_t nbytes);
int32_t write(int32_t fd, const void *buf, int32_t nbytes);
int32_t open(const uint8_t *filename);
int32_t lose(int32_t fd);
int32_t getargs(uint8_t *buf, int32_t nbytes);
int32_t vidmap(uint8_t **screen_start);
int32_t set_handler(int32_t signum, void *handler_address);
int32_t sigreturn(void);

asmlinkage int32_t sys_halt(uint8_t status);
asmlinkage int32_t sys_execute(const uint8_t *command);
asmlinkage int32_t sys_read(asmlinkage int32_t fd, void *buf, asmlinkage int32_t nbytes);
asmlinkage int32_t sys_write(asmlinkage int32_t fd, const void *buf, asmlinkage int32_t nbytes);
asmlinkage int32_t sys_open(const uint8_t *filename);
asmlinkage int32_t sys_lose(asmlinkage int32_t fd);
asmlinkage int32_t sys_getargs(uint8_t *buf, asmlinkage int32_t nbytes);
asmlinkage int32_t sys_vidmap(uint8_t **screen_start);
asmlinkage int32_t sys_set_handler(asmlinkage int32_t signum, void *handler_address);
asmlinkage int32_t sys_sigreturn(void);




#endif /* ASM */

#endif /* SYS_CALLS_H */
