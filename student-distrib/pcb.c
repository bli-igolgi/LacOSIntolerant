#include "pcb.h"

// A bitmap to optimize space in case a PCB is freed between other PCBs
// None of the upper 24 bits should be 1 unless we know we have space
uint32_t pcb_status = 0;

pcb_t * find_empty_pcb(void){
    uint32_t temp_pcb_status = pcb_status, offset = 0;
    while(__builtin_ffs(temp_pcb_status) == 1) {
        offset++;
        temp_pcb_status >>= 1;
    }
    pcb_status |= 1 << offset;
    return (pcb_t *)(END_OF_KERNEL_PAGE - PCB_PLUS_STACK*(offset+1));
}

// void done_with_pcb(uint32_t status){}

// TODO: REWRITE ABOVE FUNCTIONS SO THAT THEY DO A SIMILAR THING WITH FDs

/*
 *  Inputs: none
 *  Return Value: pointer to new process control block
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
    fdesc_t *fd;
    for(idx = 0; idx < MAX_DESC; idx++) {
        // Rename the accessor
        fd = &(blk->io_files[idx]);
        if(fd->flags != NOT_USED) continue;
        fd->file_ops = file_op;
        fd->inode = inode_num;
        // user read position always start at 0
        fd->file_pos = 0;                    
        fd->flags = IN_USE;
        return idx;
    }
    // no unused file desc was found
    return -1;
}

/*
 *  Inputs: blk     --  pointer to process control block of interest
 *          fd_id   --  file to be closed's id #
 *  Return Value: 0 on completion
 *  Function: Find file descriptor and free it
 */
int32_t close_file_desc(pcb_t *blk, uint32_t fd_id) {
    // Don't try to free if doesn't exist
    if(MAX_DESC <= fd_id) return -1;
    // note: fdesc data is not cleared; use flags as access var!
    blk->io_files[fd_id].flags = NOT_USED;
    return 0;
}
