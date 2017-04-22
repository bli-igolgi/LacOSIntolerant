#include "pcb.h"

// A bitmap to optimize space in case a PCB is freed between other PCBs
// None of the upper 24 bits should be 1 unless we know we have space
uint32_t pcb_status = 0;

/*
 *  Inputs: none
 *  Return Value: pointer to empty PCB block
 *  Function: Gets the offset to the next empty block available to use for a PCB
 */
uint32_t find_empty_pcb() {
    // Get the current PCB status, and set the current offset to 0.
    uint32_t temp_pcb_status = pcb_status, offset = 0;
    // As long as the bit 0 of the PCB status we have is set...
    while(__builtin_ffs(temp_pcb_status) == 1) {
        // Increase the offset and shift the status we have.
        offset++;
        temp_pcb_status >>= 1;
    }
    // Set the appropriate bit in the PCB status.
    pcb_status |= 1 << offset;
    // Return the offset of the pcb
    return offset;
}

// TODO: REWRITE ABOVE FUNCTIONS SO THAT THEY DO A SIMILAR THING WITH FDs

/*
 *  Inputs: none
 *  Return Value: pointer to new process control block
 *  Function: Initialize a pcb
 */
pcb_t * init_pcb() {
    uint32_t offset = find_empty_pcb();
    pcb_t *newBlk = (pcb_t *)(END_OF_KERNEL_PAGE - PCB_PLUS_STACK*(offset+1));
    // memset(newBlk, 0, 8192);
    // set the pcb number
    newBlk->pcb_num = offset;

    int i;
    
    // make all file descriptors available
    for(i = 0; i < MAX_DESC; i++)
        newBlk->io_files[i].flags = NOT_USED;
	
	// clear junk from stored arg buffer
	memset(newBlk->arg, '\0', BUF_SIZE);

    return newBlk;
}

/*
 *  Inputs: blk --  pointer to process control block
 *  Return Value: 0 on completion
 *  Function: Free all of the file descriptors associated with the PCB,
 *              and call the respective close function for each
 */
int32_t close_pcb(pcb_t *blk) {
    int i;
    for(i = 0; i < MAX_DESC; i++) {
        if(cur_pcb->io_files[i].file_ops.close)
            cur_pcb->io_files[i].file_ops.close(i);
        close_file_desc(cur_pcb, i);
    }
    return 0;
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
int32_t close_file_desc(pcb_t *blk, uint32_t fd) {
    // Don't try to free if doesn't exist
    if(MAX_DESC <= fd || fd < 0) return -1;
    cur_pcb->io_files[fd].file_ops = empty_f_ops_tab;
    // note: fdesc data is not cleared; use flags as access var!
    blk->io_files[fd].flags = NOT_USED;
    return 0;
}

/*
 * Inputs: status bitmap
 * Return Value: number of bits set in the bitmap
 * Function: Determines the Hamming weight of the bitmap.
 * (thanks Matt Howells from StackOverflow!)
 */
uint32_t our_popcount(uint32_t value) {
     uint32_t result = 0;
     // The constant below consists of every other bit set.
     result = value - ((value >> 1) & 0x55555555);
     // The constant below consists of every two bits set followed by two bits unset.
     result = (result & 0x33333333) + ((result >> 2) & 0x33333333);
     // The first constant below consists of every four bits set followed by every four bits unset.
     // The second constant below consists of every eighth bit set, starting with the first.
     return (((result + (result >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
