/*
 * Driver for the file system
 */

#include "file_system.h"

/*
 * int32_t fsys_open(const uint8_t* filename);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t fsys_open(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * int32_t fsys_close(int32_t fd);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t fsys_close(int32_t fd) {
    return SUCCESS;
}

/*
 * int32_t fsys_read_file(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t fsys_read_file(int32_t fd, void *buf, int32_t nbytes) {
    return SUCCESS;
}

/*
 * int32_t fsys_read_dir(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t fsys_read_dir(int32_t fd, void *buf, int32_t nbytes) {
    return SUCCESS;
}

/*
 * int32_t fsys_write(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t fsys_write(int32_t fd, const void *buf, int32_t nbytes) {
    return FAILURE;
}

/*
 * int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
 *   Inputs: fname -- name of file whose directory entry is to be read
             dentry -- directory entry in memory to which the above will be copied
 *   Return Value: 0 on success, -1 on failure
 *   Function: Reads a directory entry based on a given file name.
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    uint32_t num_dir_entries = fs_addr[0];
    int i = 0;
    char poss_name[33] = {0};
    // Time to find this entry.
    while(i < num_dir_entries){
        uint32_t * cur_dentry = fs_addr+(i+1)*BLK_SIZE_BYTES;
        strncpy(poss_name, cur_dentry, FILENAME_LEN);
        if(!strcmp(poss_name, fname, 32)){i++; continue;}
        memcpy(dentry, cur_dentry, 64);
        return SUCCESS;
    }
    return FAILURE;
}

/*
 * int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
 *   Inputs: index -- specific node whose information will be read
 *           dentry -- directory entry in memory to which the above will be copied
 *   Return Value: 0 on success, -1 on failure
 *   Function: Reads a directory entry based on a given index node.
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    uint32_t num_inodes = fs_addr[1];
    if(index < 0 || index >= num_inodes)
        return FAILURE;
    uint32_t * cur_dentry = fs_addr+(index+1)*BLK_SIZE_BYTES;
    memcpy(dentry, cur_dentry, 64);
    return SUCCESS;
}

/*
 * int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 *   Inputs: inode -- specific node referring to specific file
 *           offset -- how many bytes from file start we should be starting from
 *           buf -- pointer to where this data should be copied
 *           length -- length of data to be copied
 *   Return Value: 0 on success, -1 on failure
 *   Function: Reads data from a particular file given its inode and a starting point.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    uint32_t num_inodes = fs_addr[1],
             num_data_blocks = fs_addr[2];
             file_length,
             start_offset = offset%BLK_SIZE_BYTES,
             cur_block_num = offset/BLK_SIZE_BYTES,
             cur_to_do,
             remaining = length,
             done_so_far = 0;
    uint32_t* cur_block = NULL;
    uint32_t* cur_inode;
    if(inode >= num_inodes)
        return FAILURE;
    cur_inode = fs_addr + (inode+1)*BLK_SIZE_BYTES;
    file_length = cur_inode[0];
    if(offset > file_length)
        return FAILURE;
    if(offset == file_length)
        return SUCCESS;
    if(offset+remaining >= file_length)  // not all of the bytes will be copied,
        remaining += offset-file_length; // so make sure you don't copy garbage
    while(remaining > 0){
        // find first block to copy from
        cur_block = fs_addr + (num_inodes + cur_inode[cur_block_num+1])*BLK_SIZE_BYTES;
        // find how much from this block to copy
        if(done_so_far == 0)                    // copy from specific position to end of block; only happens once
            cur_to_do = 4096 - start_offset;
        else if(remaining <= 4096)
            cur_to_do = remaining;              // copy from start of block to specific position
        else
            cur_to_do = 4096;                   // copy entire block (middle of file with size > 4kB)
        // do the actual copying
        memcpy(cur_block+start_offset, buf+done_so_far, cur_to_do);
        // adjust offsets
        start_offset = 0;
        done_so_far += cur_to_do;
        remaining -= cur_to_do;
    }
    return done_so_far;
}

