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
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    return FAILURE;
}

/*
 * int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    return FAILURE;
}

/*
 * int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 *   Inputs: 
 *   Return Value: 
 *   Function: 
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    return FAILURE;
}

