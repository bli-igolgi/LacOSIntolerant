/*
 * Contains the system call definitions to run in the kernel
 */

#include "syscalls.h"

// Pointer to the PCB of the current running process
pcb_t *cur_pcb = NULL;
uint32_t numproc = 0;
f_ops_table* tableaux[3] = {&rtc_jt, &dir_jt, &regf_jt};
bool except_raised = 0;

/*
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
 int32_t sys_halt(uint8_t status) {
    int i;
    // Remap the parent's page
    // TODO: Fix below so that this works more generally with more than one process
    map_page((void *) PROGRAM_1_PHYS, (void *) PROGRAM_VIRT, true, true, true, true);
    // Zero extend the return value
    uint32_t ret_val = (status & 0xFF);
    uint32_t offset = (END_OF_KERNEL_PAGE - (uint32_t)cur_pcb)/PCB_PLUS_STACK;
    // Mark the current PCB as ready for reuse
    pcb_status &= ~(1 << (offset-1));
    // Free the file descriptors associated with the PCB
    for(i = 0; i < MAX_DESC; i++) {
        if(cur_pcb->io_files[i].file_ops.close)
            cur_pcb->io_files[i].file_ops.close(i);
        close_file_desc(cur_pcb, i);
    }

    // Go back to the parent task
    uint32_t esp_0 = cur_pcb->esp0, ss_0 = cur_pcb->ss0;
    flush_tlb();
    cur_pcb = cur_pcb->parent_task;
    // Restore the pointers to the stack
    if(cur_pcb) {
        uint32_t *cur_esp = (uint32_t *)cur_pcb->esp, *cur_ebp = (uint32_t *)cur_pcb->ebp;
//        tss.esp0 = cur_pcb->esp0;
        tss.esp0 = esp_0;
//        tss.ss0 = cur_pcb->ss0;
        tss.ss0 = ss_0;
        asm volatile (
            "movl %2, %%eax\n\
            movl %1, %%ebp\n\
            movl %0, %%esp\n\
            jmp we_are_done\n\
            "
            :
            : "g"(cur_esp), "g"(cur_ebp), "g"(ret_val)
            : "%eax"
        );
    }
    sys_execute((uint8_t *)"shell");
    return ret_val;
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
    uint32_t ret_val;
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

    // Check that the first 4 bytes match executable format
    if(strncmp((int8_t*)file_data, "\177ELF", 4)) 
        return -1;

    /* ==== Set up paging ==== */
    // TODO: Fix below so that this works more generally with more than one process
/*    if(numproc==0)
        map_page((void *) PROGRAM_1_PHYS, (void *) PROGRAM_VIRT, true, true, true, true);
    else 
        map_page((void *) PROGRAM_2_PHYS, (void *) PROGRAM_VIRT, true, true, true, true);


    //flush_tlb();
*/

    /* ==== Load file into memory ==== */
    entry = *((uint32_t *)file_data+6);
    read_data(cmd_dentry.inode_num, 0, (void *) (PROGRAM_VIRT | PROGRAM_VIRT_OFF),
                *(fs_addr + (cmd_dentry.inode_num+1)*BLK_SIZE_UINTS));
				
    /* ==== Create PCB ==== */
    pcb_t* new_pcb = init_pcb();	

    /* ==== Set up paging ==== */
    // map the process into the appropriate spot in physical memory
    map_page((void *) PROGRAM_1_PHYS + new_pcb->pcb_num * PAGE_4MB, (void *) PROGRAM_VIRT, true, true, true, true);
    flush_tlb();


/*
    // If we are spawning new task from original shell call
    if(pcb_status != 1)
        new_pcb->parent_task = cur_pcb;
    else
        new_pcb->parent_task = NULL;
*/
    // assign the parent task of the new pcb (will be NULL if this is the first process)
    new_pcb->parent_task = cur_pcb;


    new_pcb->pid = numproc++;
    new_pcb->fd_status = 3; // fd's 0 and 1 are occupied

    /* ==== Prepare for context switch ==== */
    if(cur_pcb) {
        asm volatile (
            "movl %%ebp, %1\n\
            movl %%esp, %0\n\
            "
            : "=m"(cur_pcb->esp), "=m"(cur_pcb->ebp)
        );
    }
    new_pcb->esp0 = (tss.esp0 = END_OF_KERNEL_PAGE - (new_pcb->pid)*PCB_PLUS_STACK - 4);
    new_pcb->ss0 = (tss.ss0 = KERNEL_DS);
    cur_pcb = new_pcb;
	// open default stdin (fd #0) & stdout (fd #1) per process (terminal_open uses cur_pcb!!)
	terminal_open(NULL);

    /* ==== Push IRET context to stack ==== */
    asm volatile (
        "movw $0x2B, %%ax;"
        "movw %%ax, %%ds;"
        "movw %%ax, %%es;"
        "movw %%ax, %%fs;"
        "movw %%ax, %%gs;" 

        "movl %1, %%eax;"
        "pushl $0x2B;"      // Data segment selector
        "pushl %%eax;"
        "pushf;"
        "pushl $0x23;"      // Code segment selector
        "pushl %2;"
        "iret;"
        "we_are_done:"
        "movl %%eax, %0"
        : "=m"(ret_val)
        : "r"(stackp), "r"(entry)
        : "%eax"
    );

    if(except_raised){
        ret_val = 256;
        except_raised = 0;
    }
    return ret_val;
}

/*
 *   Inputs: fd     - The file descriptor of the device
 *           buf    - The buffer to read the data into
 *           nbytes - How many bytes to read
 *   Return Value: The number of bytes read, or -1 if failed
 *   Function: Calls the read function corresponding to the device ID fd
 */
int32_t sys_read(int32_t fd, void *buf, int32_t nbytes) {
    if(!cur_pcb->io_files[fd].file_ops.read) return FAILURE;
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
    if(!cur_pcb->io_files[fd].file_ops.write) return FAILURE;
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
	
	if(cur_dentry.file_type < 3)		// 3 possible file types
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
 *   Inputs:		fd - file descriptor handler given by sys_open
 *   Return Value:	-1 if invalid descriptor or default descriptor
 *					0 otherwise
 *   Function: 		closes a file descriptor in current pcb by marking as unused
 */
int32_t sys_close(int32_t fd) {
	if(fd < 2)
		return -1;		// user cannot close default fd (0 and 1)
	else
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

