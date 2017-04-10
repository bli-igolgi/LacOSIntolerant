/*
 * Contains the system call definitions to run in the kernel
 */

#include "syscalls.h"

static bool process_1_started = false;

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
 *   Return Value: -1 if command cannot be executed
 *                 256 if program dies by exception
 *                 0 - 255 if program executes halt system call, based on return of sys_halt
 *   Function: 
 */
int32_t sys_execute(const uint8_t *command) {
    uint8_t cmd[5], arg[10], file_data[30];
    dentry_t *cmd_dentry;
    int i = 0, j = 0;
    
    /* ==== Parse arguments ==== */
    // Skip spaces in front of the command
    while(command[i] == ' ' && command[i] != '\0') i++;
    // Copy command into cmd until the next space
    while(command[i] != ' ' && command[i] != '\0') cmd[j++] = command[i++];
    cmd[j] = '\0';
    j = 0;

    // Skip spaces in front of argument
    while(command[i] == ' ' && command[i] != '\0') i++;
    // Copy the ending part of command into argument
    while(command[i] != ' ' && command[i] != '\0') arg[j++] = command[i++];
    arg[j] = '\0';

    /* ==== Check file validity ==== */
    // Make sure the name of the file is in the file system
    if(read_dentry_by_name(cmd, cmd_dentry) == -1) return -1;
    // Check that the file is executable
    read_data(cmd_dentry->inode_num + 1, 0, file_data, 30);
    if(file_data[0] != ELF_1 || file_data[1] != ELF_2 ||
       file_data[2] != ELF_2 || file_data[3] != ELF_4) {
        printf("%x, %x, %x, %x", file_data[0] == ELF_1, file_data[1] == ELF_2,
             file_data[2] == ELF_3, file_data[3] == ELF_4);
        return -1;
    }

    /* ==== Set up paging ==== */
    if(!process_1_started) {
        map_page((void *) PROGRAM_1_PHYS, (void *) PROGRAM_VIRT, true, true, true);
        process_1_started = true;
    }
    else 
        map_page((void *) PROGRAM_2_PHYS,(void *) PROGRAM_VIRT, true, true, true);

    /* ==== Load file into memory ==== */

    /* ==== Create PCB ==== */

    /* ==== Open FDs ==== */

    /* ==== Prepare for context switch ==== */

    /* ==== Push IRET context to switch ==== */

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

