
#include "pcb.h"

// Section below is file_ops jump tables for type-specific open(0), read(1), write(2), and close(3)
void (*stdin[4]) = { NULL, terminal_read, NULL, NULL };     // read-only
void (*stdout[4]) = { NULL, NULL, terminal_write, NULL };   // write-only
void (*dir_jt[4]) = { fsys_open_dir, fsys_read_dir, fsys_write_dir, fsys_close_dir };
void (*regf_jt[4]) = { fsys_open_file, fsys_read_file, fsys_write_file, fsys_close_file };
void (*rtc_jt[4]) = { rtc_open, rtc_read, rtc_write, rtc_close };


// A bitmap to optimize space in case a PCB is freed between other PCBs.
// (This will be adapted to opening and closing files later.)
uint32_t pcb_status = 0; // none of the upper 24 bits should be 1 unless we know we have space

/*
 *  Inputs: newBlk  -- pointer to new process control block
 *  Return Value: none
 *  Function: Initialize a pcb with default file descriptors
 */
void init_pcb(pcb_t *newBlk) {
    int i;
    
    // make all file descriptors available
    for(i = 0; i < MAX_DESC; i++)
        (*newBlk).io_files[i].flags = NOT_USED;
    
    // automatically open stdin (fd #0) & stdout (fd #1)
    open_file_desc(newBlk, stdin, 1, 0);
    open_file_desc(newBlk, stdout, 1, 0);

    // PID PCB starts at 4MB - (PID+1)*8kB, goes towards bigger memory addresses
    // memcpy((void *)(FOUR_MB - (pid+1)*EIGHT_KB), newBlk, );
}

/*
 *  Inputs: blk         --  pointer to process control block
 *          file_op     --  file operation jump table associated with file type
 *          file_type   --  similar to file system: 0 for RTC, 1 for directory, 2 for reg files
 *          inode_num   --  inode number for this data file
 *  Return Value: 0 - 7 on success, -1 on failure
 *  Function: Dynamically assign an available file descriptor. Returns the file descriptor id# (io_file index)
 *              upon allocation success; fails if array is full / all file desc are occupied.
 */
int32_t open_file_desc(pcb_t *blk, void *file_op, uint32_t file_type, uint32_t inode_num) {
    int idx;
    fdesc_t fd;
    for(idx = 0; idx < MAX_DESC; idx++)
        if((*blk).io_files[idx].flags == NOT_USED) {
            fd = (*blk).io_files[idx];
            fd.file_ops = (uint32_t *)file_op;  // jmp table exists for every file type
            
            if(file_type == 2)
                fd.inode = inode_num;
            else
                fd.inode = 0;                   // no inode # for directory or RTC device files
            
            fd.file_pos = 0;                    // user read position always start at 0
            fd.flags = IN_USE;
            return idx;
        }
    
    //  no unused file desc was found
    return -1;
}

/*
 *  Inputs: blk     --  pointer to process control block of interest
 *          fd_id   --  file to be closed's id #
 *  Return Value: 0 on success, -1 on failure
 *  Function: Find file descriptor and free it, if it is in-use / valid (except for stdin & stdout)
 */
int32_t close_file_desc(pcb_t *blk, uint32_t fd_id) {
    if(1 < fd_id && fd_id < MAX_DESC) {
        (*blk).io_files[fd_id].flags = NOT_USED;        // note: fdesc data is not cleared; use flags as access var!
        return 0;
    } else
        return -1;
}

/*
pcb_t * find_empty_pcb(void){
    uint32_t temp_pcb_status = pcb_status, offset = 0;
    while(__builtin_ffs(temp_pcb_status) == 1){
        offset++; temp_pcb_status >>= 1;
    }
    pcb_status |= 0x01 << offset;
    return (pcb_t *)(END_OF_KERNEL_PAGE - PCB_PLUS_STACK*(offset+1));
}

void done_with_pcb(pcb_t* old_pcb){
    // TOFIX
    uint32_t offset = (uint32_t)(END_OF_KERNEL_PAGE - (uint8_t *)old_pcb)/PCB_PLUS_STACK;
    pcb_status &= ~(0x01 << (offset-1));
    return;
}*/
