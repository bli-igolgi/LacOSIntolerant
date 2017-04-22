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
 *   Inputs: status - the 8 bit return value
 *   Return Value: 
 *   Function: Closes the respective process, restarting if the first process was halted
 */
 int32_t sys_halt(uint8_t status) {
    // Zero extend the return value
    uint32_t ret_val = (status & 0xFF),
             offset = (END_OF_KERNEL_PAGE - (uint32_t)cur_pcb)/PCB_PLUS_STACK;

    // Remap to the parent's page
    if(cur_pcb->parent_task)
        map_page((void *) cur_pcb->parent_task->page_addr, (void *) PROGRAM_VIRT, true, true, true, true);
    else
        map_page((void *) PROGRAM_1_PHYS + cur_pcb->term_num * FOUR_MB, (void *) PROGRAM_VIRT, true, true, true, true);
    // Mark the current PCB as ready for reuse
    pcb_status &= ~(1 << (offset-1));
    close_pcb(cur_pcb);

    flush_tlb();
    // Go back to the parent task
    cur_pcb = cur_pcb->parent_task;
    // Restore the pointers to the stack
    if(cur_pcb) {
        tss.esp0 = cur_pcb->esp0;
        tss.ss0 = cur_pcb->ss0;
        // Restores the esp and ebp, and puts return value in eax
        asm volatile (
            "movl %2, %%eax;"
            "movl %1, %%ebp;"
            "movl %0, %%esp;"
            "jmp we_are_done;"
            :
            : "g"(cur_pcb->esp), "g"(cur_pcb->ebp), "g"(ret_val)
            : "%eax"
        );
    } else
        sys_execute((uint8_t *)"shell");
    
    // This should never be reached
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
    uint8_t cmd[15], file_data[FILE_H_SIZE];
    uint32_t entry = 0;
    dentry_t cmd_dentry;
    int i = 0, j = 0;
    uint32_t ret_val;
    // Set the stack pointer to be just before the bottom of the page
    uint32_t stackp = PROGRAM_VIRT + FOUR_MB - 0x4;


    // TEMPORARY: Only allow there to be 2 running tasks
    if(our_popcount(pcb_status) == MAX_PROCESSES) {
        printf("Only %d tasks are currently supported\n", MAX_PROCESSES);
        return 0;
    }

    /* ==== Parse command ==== */
    // Skip spaces in front of the command
    while(command[i] == ' ' && command[i] != '\0') i++;
    // Copy command into cmd until the next space
    while(command[i] != ' ' && command[i] != '\0') cmd[j++] = command[i++];
    cmd[j] = '\0';
    j = 0;
    
    /* ==== Check file validity ==== */
    // Make sure the name of the file is in the file system
    if(read_dentry_by_name(cmd, &cmd_dentry) == -1) return -1;
    // Read the first 30 bytes into file_data
    read_data(cmd_dentry.inode_num, 0, file_data, FILE_H_SIZE);

    // Check that the first 4 bytes match executable format
    if(strncmp((int8_t*)file_data, "\177ELF", 4)) 
        return -1;

    /* ==== Create PCB ==== */
    pcb_t* new_pcb = init_pcb();    

    /* ==== Store arg in PCB ==== */
    // Skip spaces in front of argument
    while(command[i] == ' ' && command[i] != '\0') i++;
    // Copy the ending part of command into argument
    while(command[i] != '\0') 
        new_pcb->arg[j++] = command[i++];
    new_pcb->arg[j] = '\0';
    
    /* ==== Set up paging ==== */
    // Map the process into the appropriate spot in physical memory
    new_pcb->page_addr = (uint32_t *)(PROGRAM_1_PHYS + new_pcb->pcb_num * FOUR_MB);
    map_page((void *)(new_pcb->page_addr), (void *)PROGRAM_VIRT, true, true, true, true);
    flush_tlb();

    // Assign the parent task of the new pcb (will be NULL if this is the first process)
    new_pcb->parent_task = cur_pcb;
    new_pcb->pid = numproc++;
    new_pcb->fd_status = 3; // fd's 0 and 1 are occupied
    new_pcb->esp0 = (tss.esp0 = END_OF_KERNEL_PAGE - (new_pcb->pcb_num)*PCB_PLUS_STACK - 4);
    new_pcb->ss0 = (tss.ss0 = KERNEL_DS);
    new_pcb->term_num = 0; // this will change when we work on virtual terminals

    /* ==== Prepare for context switch ==== */
    if(cur_pcb) {
        // Saves the current esp and ebp in the parent task
        asm volatile (
            "movl %%ebp, %1;"
            "movl %%esp, %0;"
            : "=m"(cur_pcb->esp), "=m"(cur_pcb->ebp)
        );
    }
    // Switch to the child task
    terminals[cur_term_id].cur_task = new_pcb;
    cur_pcb = new_pcb;
    // Open default stdin (fd #0) & stdout (fd #1) per process
    terminal_open(NULL);

    /* ==== Load file into memory ==== */
    entry = *((uint32_t *)file_data + 6);
    // Load the executable into memory starting at 0x8048000
    read_data(cmd_dentry.inode_num, 0, (void *) (PROGRAM_VIRT + PROGRAM_VIRT_OFF),
                *(fs_addr + (cmd_dentry.inode_num+1)*BLK_SIZE_UINTS));

    /* ==== Push IRET context to stack ==== */
    asm volatile (
        "movw $0x2B, %%ax;"
        "movw %%ax, %%ds;"
        "movw %%ax, %%es;"
        "movw %%ax, %%fs;"
        "movw %%ax, %%gs;" 

        "movl %1, %%eax;"
        "pushl $0x2B;"      // Data segment selector
        "pushl %%eax;"      // ESP
        "pushf;"
        "pushl $0x23;"      // Code segment selector
        "pushl %2;"         // EIP
        "iret;"
        "we_are_done:"
        "movl %%eax, %0"
        : "=m"(ret_val)
        : "r"(stackp), "r"(entry)
        : "%eax"
    );

    if(except_raised) {
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
    // Check that the file descriptor is in range and the read function exists
    if(fd < 0 || fd >= MAX_DESC || !cur_pcb->io_files[fd].file_ops.read)
        return -1;
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
    // Check that the file descriptor is in range and the write function exists
    if(fd < 0 || fd >= MAX_DESC || !cur_pcb->io_files[fd].file_ops.write)
        return -1;
    return cur_pcb->io_files[fd].file_ops.write(fd, buf, nbytes);
}

/*
 *   Inputs: filename - the string that contains the name of the file
 *   Return Value: -1 if filename does not exist or specific file_open fails
 *                  0 - 7 file descriptor id# (fd) 
 *   Function: Checks the filename, and calls the file type specific open function
 */
int32_t sys_open(const uint8_t *filename) {
    dentry_t cur_dentry;
    
    if(read_dentry_by_name(filename, &cur_dentry))
        return -1;
    
    if(cur_dentry.file_type < 3)        // 3 possible file types
        return (*tableaux[cur_dentry.file_type]).open(filename);
    else
        return -1;
}

/*
 *   Inputs: fd - file descriptor handler given by sys_open
 *   Return Value: -1 if invalid descriptor or default descriptor, 0 otherwise
 *   Function: closes a file descriptor in current pcb by marking as unused
 */
int32_t sys_close(int32_t fd) {
    // User cannot close default fd (0 and 1)
    if(fd < 2 || fd >= MAX_DESC || !cur_pcb->io_files[fd].file_ops.close)
        return -1;
    return cur_pcb->io_files[fd].file_ops.close(fd);
}

/*
 *   Inputs: buf    - buffer to copy arg into
 *           nbytes - number of bytes to copy into buf
 *   Return Value: -1 if null-terminated arg does not fit in buf, 0 otherwise
 *   Function: get stored arg parsed from command at time of sys_execute
 */
int32_t sys_getargs(uint8_t *buf, int32_t nbytes) {
    int32_t arg_len;
    
    // return if arg and NULL char cannot fit in buffer
    if((arg_len = strlen((int8_t *)cur_pcb->arg)+1) > nbytes)
        return -1;
    
    memcpy(buf, cur_pcb->arg, arg_len);
    return 0;
}

/*
 *   Inputs: The address to map
 *   Return Value: The address that was mapped
 *   Function: Maps the text-mode video memory into user space
 *              at a pre-set virtual address
 */
int32_t sys_vidmap(uint8_t **screen_start) {
    // The location needs to fall within the program executable page
    if(screen_start < (uint8_t **)PROGRAM_VIRT ||
        screen_start > (uint8_t **)(PROGRAM_VIRT + FOUR_MB)) {
        return -1;
    }
    // Map the address provided to the same virtual address
    *screen_start = (void *)(VIDMAP_VIRT_ADDR);

    return VIDMAP_VIRT_ADDR;
}

/*   Unless we decide to implement signals, this is as it will remain.
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_set_handler(int32_t signum, void *handler_address) {
    return -1;
}

/*   Unless we decide to implement signals, this is as it will remain.
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t sys_sigreturn(void) {
    return -1;
}

