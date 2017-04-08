#include "syscalls.h"

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_halt(uint8_t status) {
    printf("executed syscall ");
    // Zero extend the value
    return (status & 0xFF);
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_execute(const uint8_t *command) {
    uint8_t arg[15];
    uint8_t cmd[15];
    int i = 0, j = 0;
    // Skip spaces in front of the command
    while(command[i] == ' ' && command[i] != '\0') { i++; }
    while(command[i] != ' ' && command[i] != '\0') {
        cmd[j++] = command[i++];
    }
    cmd[j] = '\0';
    j = 0;
    // Skip spaces in front of argument
    while(command[i] == ' ' && command[i] != '\0') { i++; }
    while(command[i] != ' ' && command[i] != '\0') {
        arg[j++] = command[i++];
    }
    arg[j] = '\0';

    printf("sys_execute, command: %s, cmd: %s, arg: %s\n", command, cmd, arg);
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_read(int32_t fd, void *buf, int32_t nbytes) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_write(int32_t fd, const void *buf, int32_t nbytes) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_open(const uint8_t *filename) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_close(int32_t fd) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_getargs(uint8_t *buf, int32_t nbytes) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_vidmap(uint8_t **screen_start) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_set_handler(int32_t signum, void *handler_address) {
    printf("executed syscall ");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_sigreturn(void) {
    printf("executed syscall ");
    return -1;
}

