/*
 * Contains the system call definitions to run in the kernel
 */

#include "syscalls.h"

static bool process_1_started = false;

// Pointer to the PCB of the current running process
pcb_t *cur_pcb = NULL;

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_halt(uint8_t status) {
    printf("executed syscall halt\n");

    // Restart first process if halt is called on it
    if(pcb_status == 1) sys_execute((uint8_t *)"shell");
    // Zero extend the value
    return (status & 0xFF);
}

/*
 *   Inputs: command - a string containing the command to be executed, along with arguments
 *   Return Value: -1 if command cannot be executed
 *                 256 if program dies by exception
 *                 0 - 255 if program executes halt system call, based on return of sys_halt
 *   Function: Performs the command
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

    /*
    for(i = 0; i < FILE_H_SIZE; i++) {
        printf("%x",file_data[i]);
        if(i) {
            if(i % 2 == 1) printf(" ");
            if((i+1) % 16 == 0) printf("\n");
        }
    }
    printf("\n");
    */

    // Check that the first 4 bytes match executable format
    if(strncmp((int8_t*)file_data, "\177ELF", 4)) 
        return -1;

    /* ==== Set up paging ==== */
    /*
    TODO: Set up new page directory
    TODO: Fix below so that this works more generally with more than one process
    */
    if(!process_1_started) {
        map_page((void *) PROGRAM_1_PHYS, (void *) PROGRAM_VIRT, true, true, true);
        process_1_started = true;
    }
    else 
        map_page((void *) PROGRAM_2_PHYS, (void *) PROGRAM_VIRT, true, true, true);

    /* ==== Load file into memory ==== */
    entry = *((uint32_t *)file_data+6);
    printf("entry: %x\n", entry);
    read_data(cmd_dentry.inode_num, 0, (void *) (PROGRAM_VIRT | PROGRAM_VIRT_OFF),
                *(fs_addr + (cmd_dentry.inode_num+1)*BLK_SIZE_UINTS));

    /* ==== Create PCB ==== */
    pcb_t* new_pcb = init_pcb();
    // If we are spawning new task from original shell call
    if(pcb_status != 1)
        new_pcb->parent_task = cur_pcb;
    else
        new_pcb->parent_task = NULL;
    new_pcb->fd_status = 3; // fd's 0 and 1 are occupied
    if(!cur_pcb)
        cur_pcb = new_pcb;

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
        "ret;"
        : 
        : "r"(stackp), "r"(entry)
        : "%eax"
    );

    // printf("sys_execute, command: %s, cmd: %s, arg: %s\n", command, cmd, arg);
    return 0;
}
f_ops_table* tableaux[3] = {&rtc_jt, &dir_jt, &regf_jt};

/*
 *   Inputs: fd     - The file descriptor of the device
 *           buf    - The buffer to read the data into
 *           nbytes - How many bytes to read
 *   Return Value: The number of bytes read, or -1 if failed
 *   Function: Calls the read function corresponding to the device ID fd
 */
int32_t sys_read(int32_t fd, void *buf, int32_t nbytes) {
    return cur_pcb->io_files[fd].file_ops.read(fd, buf, nbytes);
}

/*
 *   Inputs: fd     - The file descriptor of the device
 *           buf    - The data to write
 *           nbytes - How many bytes to write
 *   Return Value: The number of bytes written
 *   Function: Writes data to the specified device
 */
int32_t sys_write(int32_t fd, const void *buf, int32_t nbytes) {
    return cur_pcb->io_files[fd].file_ops.write(fd, buf, nbytes);
}

/*
 *   Inputs:		filename - the string that contains the name of the file
 *   Return Value:	-1 if filename does not exist or specific file_open fails
 *					0 - 7 file descriptor id# (fd) 
 *   Function:		Checks the filename, and calls the file type specific open function
 */
int32_t sys_open(const uint8_t *filename) {
    dentry_t cur_dentry;
	
    if(read_dentry_by_name(filename, &cur_dentry))
		return FAILURE;
	
	if(cur_dentry.file_type < 3)		// 3 is the number of possible file types
		return (*tableaux[cur_dentry.file_type]).open(filename);
	else
		return FAILURE;
	/*switch(cur_dentry.file_type){
		case RTC_DEV:
			return rtc_open(filename);
		case DIR_TYPE:
			return fsys_open_dir(filename);
		case REG_FILE:
			return fsys_open_file(filename);
		default:
			return FAILURE;
	} */
}

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_close(int32_t fd) {
	return cur_pcb->io_files[fd].file_ops.close(fd);
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

