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
    uint8_t cmd[15], arg[30], file_data[FILE_H_SIZE];
    uint32_t entry = 0;
    dentry_t cmd_dentry;
    int i = 0, j = 0;
    // Set the stack pointer to be just before the bottom of the page
    uint32_t stackp = PROGRAM_VIRT + FOUR_MB - 0x4;
    
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
    while(command[i] != '\0') arg[j++] = command[i++];
    arg[j] = '\0';

    /* ==== Check file validity ==== */
    // Make sure the name of the file is in the file system
    if(read_dentry_by_name(cmd, &cmd_dentry) == -1) return -1;
    // Check that the file is executable
    read_data(cmd_dentry.inode_num, 0, file_data, FILE_H_SIZE);

    for(i = 0; i < FILE_H_SIZE; i++) {
        printf("%x",file_data[i]);
        if(i) {
            if(i % 2 == 1) printf(" ");
            if((i+1) % 16 == 0) printf("\n");
        }
    }
    printf("\n");

    // Check that the first 4 bytes match executable format
    if(strncmp((int8_t*)file_data, "\177ELF", 4)) 
        return -1;

    /* ==== Set up paging ==== */
    /*
    TODO: Set up new page directory
    */
    if(!process_1_started) {
        map_page((void *) PROGRAM_1_PHYS, (void *) PROGRAM_VIRT, true, true, true);
        process_1_started = true;
    }
    else 
        map_page((void *) PROGRAM_2_PHYS, (void *) PROGRAM_VIRT, true, true, true);

    /* ==== Load file into memory ==== */
    entry = (file_data[27] << 24) | (file_data[26] << 16) | (file_data[25] << 8) | file_data[24];
    printf("entry: %x\n", entry);
    read_data(cmd_dentry.inode_num, 0, (void *) (PROGRAM_VIRT | PROGRAM_VIRT_OFF),
                *(fs_addr + (cmd_dentry.inode_num+1)*BLK_SIZE_UINTS));

    /* ==== Create PCB ==== */
    init_pcb(&cur_pcb);
    /*pcb_t* new_pcb = find_empty_pcb();
    memset(new_pcb, 0xA5, sizeof(pcb_t));
    if(!no_other_processes)
        new_pcb->parent_task = cur_pcb; // gotta fix this
    else
        new_pcb->parent_task = NULL;
    */
    /* ==== Open default FDs ==== */
    /* new_pcb->io_files[0] = (*stdin);
       new_pcb->io_files[1] = (*stdout);
    */

    /* ==== Prepare for context switch ==== */

    // tss.esp0 = PROGRAM_1_PHYS - 0x4;
    // ltr(KERNEL_TSS);

    /* ==== Push IRET context to stack ==== */
    asm volatile (
        "movw $0x2B, %%ax;"
        "movw %%ax, %%ds;"
        "movw %%ax, %%es;"
        "movw %%ax, %%fs;"
        "movw %%ax, %%gs;" 

        "movl %0, %%eax;"
        "pushl $0x2B;"      // Data segment selector
        "pushl %%eax;"
        "pushf;"
        "pushl $0x23;"      // Code segment selector
        "pushl %1;"
        "iret;"
        : 
        : "r"(stackp), "r"(entry)
        : "%eax"
    );

    printf("sys_execute, command: %s, cmd: %s, arg: %s\n", command, cmd, arg);
    return 0;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_read(int32_t fd, void *buf, int32_t nbytes) {
    // printf("executed syscall read");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_write(int32_t fd, const void *buf, int32_t nbytes) {
    printf("executed syscall write");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_open(const uint8_t *filename) {
    printf("executed syscall open");
    return -1;
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_close(int32_t fd) {
    printf("executed syscall close");
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

/*   Unless we decide to implement signals, this is as it will remain.
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_set_handler(int32_t signum, void *handler_address) {
    printf("executed syscall ");
    return -1;
}

/*   Unless we decide to implement signals, this is as it will remain.
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_sigreturn(void) {
    printf("executed syscall ");
    return -1;
}

