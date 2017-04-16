
#include "pcb.h"

// A bitmap to optimize space in case a PCB is freed between other PCBs.
uint32_t pcb_status = 0; // none of the upper 24 bits should be 1 unless we know we have space

pcb_t * find_empty_pcb(void){
    uint32_t temp_pcb_status = pcb_status, offset = 0;
    while(__builtin_ffs(temp_pcb_status) == 1){
        offset++; temp_pcb_status >>= 1;
    }
    pcb_status |= 0x01 << offset;
    return (pcb_t *)(END_OF_KERNEL_PAGE - PCB_PLUS_STACK*(offset+1));
}

void done_with_pcb(pcb_t* old_pcb){
    uint32_t offset = (END_OF_KERNEL_PAGE - (uint32_t)old_pcb)/PCB_PLUS_STACK;
    pcb_status &= ~(0x01 << (offset-1));
    return;
}

// TODO: REWRITE ABOVE FUNCTIONS SO THAT THEY DO A SIMILAR THING WITH FDs

/*
 *  Inputs: newBlk  -- pointer to new process control block
 *  Return Value: none
 *  Function: Initialize a pcb
 */
pcb_t * init_pcb() {
    pcb_t *newBlk = find_empty_pcb();
    int i;
    
    // make all file descriptors available
    for(i = 0; i < MAX_DESC; i++)
        newBlk->io_files[i].flags = NOT_USED;

    return newBlk;
}

/*
 *  Inputs: blk         --  pointer to process control block
 *          file_op     --  file operation jump table associated with file type
 *          inode_num   --  inode number for this data file
 *  Return Value: 0 - 7 on success, -1 on failure
 *  Function: Dynamically assign an available file descriptor. Returns the file descriptor id# (io_file index)
 *              upon allocation success; fails if array is full / all file desc are occupied.
 */
int32_t open_file_desc(pcb_t *blk, f_ops_table file_op, uint32_t inode_num) {
    int idx;
    fdesc_t fd;
    for(idx = 0; idx < MAX_DESC; idx++)
        if((*blk).io_files[idx].flags == NOT_USED) {
            fd = (*blk).io_files[idx];
            fd.file_ops = file_op;  // jmp table exists for every file type
            
			fd.inode = inode_num;	// inode # passed in should be 0 for directory or RTC device files
            
            fd.file_pos = 0;                    // user read position always start at 0
            fd.flags = IN_USE;
            (*blk).io_files[idx] = fd;
            return idx;
        }
    
    // no unused file desc was found
    return -1;
}

/*
 *  Inputs: blk     --  pointer to process control block of interest
 *          fd_id   --  file to be closed's id #
 *  Return Value: 0 on completion
 *  Function: Find file descriptor and free it (assumes bound check was already done by general sys_close)
 */
int32_t close_file_desc(pcb_t *blk, uint32_t fd_id) {
    (*blk).io_files[fd_id].flags = NOT_USED;        // note: fdesc data is not cleared; use flags as access var!
    return SUCCESS;
}
