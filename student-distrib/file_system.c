/*
 * Driver for the file system
 */

#include "file_system.h"
#include "keyboard.h"

// Until the process control blocks are set up for each process, these are just stubs.

/*
 * int32_t fsys_open_file(const uint8_t* filename);
 *   Inputs: filename -- name of file to be opened
 *   Return Value: 0 on success (every time at this point)
 *   Function: system call to open a file
 */
int32_t fsys_open_file(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * int32_t fsys_close_file(int32_t fd);
 *   Inputs: fd -- file descriptor of open file
 *   Return Value: 0 on success (every time at this point)
 *   Function: system call to close a file
 */
int32_t fsys_close_file(int32_t fd) {
    return SUCCESS;
}

/*
 * int32_t fsys_open_dir(const uint8_t* filename);
 *   Inputs: filename -- name of directory to be opened
 *   Return Value: 0 on success (every time at this point)
 *   Function: system call to open a directory
 */
int32_t fsys_open_dir(const uint8_t* filename) {
    return SUCCESS;
}

/*
 * int32_t fsys_close_dir(int32_t fd);
 *   Inputs: fd -- file descriptor of open directory
 *   Return Value: 0 on success (every time at this point)
 *   Function: system call to close a directory
 */
int32_t fsys_close_dir(int32_t fd) {
    return SUCCESS;
}

/*
 * int32_t fsys_read_file(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     -- file descriptor of open file
 *           buf    -- pointer to location to which data will be copied
 *           nbytes -- number of bytes to be copied
 *   Return Value: number of bytes read (which is 0 at every point)
 *   Function: system call to read from a file
 */
int32_t fsys_read_file(int32_t fd, void *buf, int32_t nbytes) {
    // This does not work without actual file descriptors.
    return SUCCESS;
}

/*
 * int32_t fsys_read_dir(int32_t fd, void *buf, int32_t nbytes);
 *   Inputs: fd     -- file descriptor of given directory
 *           buf    -- pointer to location to which data will be copied
 *           nbytes -- number of bytes to be copied
 *   Return Value: 0 on success (every time at this point)
 *   Function: system call to read from a directory
 */
int32_t fsys_read_dir(int32_t fd, void *buf, int32_t nbytes) {
    // This does not work without actual file descriptors.
    return SUCCESS;
}

/*
 * int32_t fsys_write_file(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd -- file descriptor of given file
 *           buf -- pointer to location to which data will be copied
 *           nbytes -- number of bytes to be copied
 *   Return Value: -1 on failure (every time at this point)
 *   Function: system call to write to a file
 */
int32_t fsys_write_file(int32_t fd, const void *buf, int32_t nbytes) {
    return FAILURE; // 3.2: Parse the _Read-Only_ File System
}

/*
 * int32_t fsys_write_dir(int32_t fd, const void *buf, int32_t nbytes);
 *   Inputs: fd     -- file descriptor of given directory
 *           buf    -- pointer to location to which data will be copied
 *           nbytes -- number of bytes to be copied
 *   Return Value: -1 on failure (every time at this point)
 *   Function: system call to write to a directory
 */
int32_t fsys_write_dir(int32_t fd, const void *buf, int32_t nbytes) {
    return FAILURE; // 3.2: Parse the _Read-Only_ File System
}

bool check_file_name_exists(const uint8_t *fname) {
    uint32_t num_dir_entries = fs_addr[0];
    int i = 0;
    char pos_name[FILENAME_LEN + 1];
    if(strlen((int8_t *)fname) > FILENAME_LEN)
        return FAILURE;
    // Time to find this entry.
    for(i = 0; i < num_dir_entries; i++) {
        // Find the right address, offsetting by 1 because inode0 is null
        uint32_t * cur_dentry = fs_addr + (i+1) * ENTRY_SIZE_UINTS;
        // Put the filename in poss_name
        strncpy(pos_name, (int8_t *)cur_dentry, FILENAME_LEN);
        // If the strings are the same
        if(!strncmp(pos_name, (int8_t *)fname, strlen(pos_name)))
            return SUCCESS;
    }
    // Could not find the filename
    return FAILURE;
}

/*
 * int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
 *   Inputs: fname  -- name of file whose directory entry is to be read
             dentry -- directory entry in memory to which the above will be copied
 *   Return Value: 0 on success, -1 on failure
 *   Function: Reads a directory entry based on a given file name.
 */
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry) {
    // Initialize number of directory entries, a counter, and a buffer for prospective filenames.
    uint32_t num_dir_entries = fs_addr[0];
    int i = 0;
    char poss_name[FILENAME_LEN + 1] = {0};
    if(strlen((int8_t *)fname) > FILENAME_LEN)
        return FAILURE;
    // Time to find this entry.
    while(i < num_dir_entries){
        // Find the right address, offsetting by 1 because inode0 is null
        uint32_t * cur_dentry = fs_addr + (i+1) * ENTRY_SIZE_UINTS;
        // Put the filename in poss_name
        strncpy(poss_name, (int8_t *)cur_dentry, FILENAME_LEN);
        // If the strings are not the same
        if(strncmp(poss_name, (int8_t *)fname, strlen(poss_name))) {
            i++;
            continue;
        }
        // Copy the directory entry and be done!
        memcpy(dentry, cur_dentry, ENTRY_SIZE_BYTES);
        return SUCCESS;
    }
    // Could not find the filename
    return FAILURE;
}

/*
 * int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
 *   Inputs: index  -- specific node whose information will be read
 *           dentry -- directory entry in memory to which the above will be copied
 *   Return Value: 0 on success, -1 on failure
 *   Function: Reads a directory entry based on a given index node.
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry) {
    // Get number of inodes.
    uint32_t num_inodes = fs_addr[1];
    // Fail if given index is not in the valid range.
    if(index >= num_inodes)
        return FAILURE;
    // Find the appropriate dentry, copy it, and be done!
    uint32_t * cur_dentry = fs_addr + (index+1) * ENTRY_SIZE_UINTS;
    memcpy(dentry, (dentry_t *)cur_dentry, ENTRY_SIZE_BYTES);
    return SUCCESS;
}

/*
 * int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 *   Inputs: inode  -- specific node referring to specific file
 *           offset -- how many bytes from file start we should be starting from
 *           buf    -- pointer to where this data should be copied
 *           length -- length of data to be copied
 *   Return Value: 0 on success, -1 on failure
 *   Function: Reads data from a particular file given its inode and a starting point.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
    // Set up...
    uint32_t num_inodes = fs_addr[1],                //           the number of inodes,
             num_blocks = fs_addr[2],                //           the number of data blocks,
             file_length,                            // space for the length of the file,
             start_offset = offset%BLK_SIZE_BYTES,   //           the starting offset (for each block),
             cur_block_num = offset/BLK_SIZE_BYTES,  //           the starting block number,
             cur_to_do,                              //           the number of bytes to copy from a block,
             remaining = length,                     //           the number of remaining bytes to copy,
             done_so_far = 0;                        //       and the number of bytes copied so far.
    // Set up pointers to the current block and inode.
    uint8_t* cur_block = NULL;
    uint32_t* cur_inode;
    // Fail if inode index is not in valid range.
    if(inode >= num_inodes)
        return FAILURE;
    // Find the appropriate inode, get the file length, and be done if offset is not in valid range.
    cur_inode = fs_addr + (inode+1)*BLK_SIZE_UINTS;
    file_length = cur_inode[0];
    if(offset >= file_length)
        return SUCCESS;
    // Truncate if necessary.
    if(offset + remaining > file_length)   // if this is true, then not all of the bytes will be copied,
        remaining += offset-file_length; // so make sure you don't copy garbage
    // Check that a bad data block isn't present.
    if(cur_inode[cur_block_num+1] >= num_blocks)
        return FAILURE;
    while(remaining > 0) {
        // find first block to copy from
        if(cur_inode[cur_block_num+1] >= num_blocks)
            break;
        cur_block = (uint8_t *)fs_addr + (num_inodes + cur_inode[cur_block_num+1] + 1)*BLK_SIZE_BYTES;
        // find how much from this block to copy
        if(done_so_far == 0) { // only happens once
            // copy from specific position to end of block if we need more than one block
            if(start_offset+remaining > BLK_SIZE_BYTES)
                cur_to_do = BLK_SIZE_BYTES - start_offset;
            // else just do the whole thing
            else
                cur_to_do = remaining;
        }
        else if(remaining <= BLK_SIZE_BYTES)
            cur_to_do = remaining;      // copy from start of block to specific position
        else
            cur_to_do = BLK_SIZE_BYTES; // copy entire block (middle of file with size > 4kB)
        // do the actual copying
        memcpy(buf+done_so_far, cur_block+start_offset, cur_to_do);
        // adjust offsets
        start_offset = 0;
        done_so_far += cur_to_do;
        remaining -= cur_to_do;
        cur_block_num++;
    }
    // be done
    return done_so_far;
}

void test_access_by_index(){
	dentry_t to_print;
	char becausenonullisguaranteed[FILENAME_LEN+1];
	becausenonullisguaranteed[FILENAME_LEN] = '\0';
	uint32_t num_of_files = fs_addr[0];
   	int i, file_size;
    clear();
    clear_buffer();
    // Read all of the files
   	for(i = 0; i < num_of_files; ++i){
		read_dentry_by_index(i, &to_print);
        // Copy to string because needs to be null terminated
		memcpy(becausenonullisguaranteed, to_print.file_name, FILENAME_LEN);
        file_size = fs_addr[(to_print.inode_num+1)*BLK_SIZE_UINTS];
		printf("%s, %u, %u\n", becausenonullisguaranteed, to_print.file_type, file_size);
	}
}

void test_access_by_file_name(){
	char* filename = "frame1.txt";
    int are_we_good;
	dentry_t to_print;
	char temp_string[NUM_COLS+1] = {0}; // plus 1 for a guaranteed null terminator
	uint32_t file_size, printed = 0, how_much, i;
    clear();
    clear_buffer();
	read_dentry_by_name((uint8_t *)filename, &to_print);
	file_size = fs_addr[(to_print.inode_num+1)*BLK_SIZE_UINTS]; // over 4 because uint32_t indices
	while(file_size > 0) {
		// this is pretty dreadful but we don't have malloc yet
        how_much = (file_size>=NUM_COLS)?NUM_COLS:file_size;
		if((are_we_good = read_data(to_print.inode_num, printed, (uint8_t *)temp_string, how_much)) == FAILURE)
            break;
        if(how_much < NUM_COLS)
            memset(temp_string+how_much, 0, NUM_COLS-how_much);
		for(i = 0; i < how_much; ++i)
            putc(temp_string[i]);
		file_size -= are_we_good;
		printed += are_we_good;
	}
	printf("\n%s", to_print.file_name);
}

void test_data_printing(){
	static dentry_t to_print;
	int num_of_files = fs_addr[0], howmuchtoprint, are_we_good;
	static int index_to_be_printed = 0;
	uint32_t file_size, printed = 0, i;
	char temp_string[NUM_COLS+1] = {0}; // plus 1 for a guaranteed null terminator
	char becausenonullisguaranteed[33];
	becausenonullisguaranteed[32] = '\0';
    clear();
    clear_buffer();
	index_to_be_printed++;
	index_to_be_printed %= num_of_files;
	read_dentry_by_index(index_to_be_printed, &to_print);
	file_size = fs_addr[(to_print.inode_num+1)*BLK_SIZE_UINTS];
	while(file_size > 0) {
		// this is pretty dreadful but we don't have malloc yet
        howmuchtoprint = (file_size>=NUM_COLS)?NUM_COLS:file_size;
		if((are_we_good = read_data(to_print.inode_num, printed, (uint8_t *)temp_string, howmuchtoprint)) == FAILURE)
            break;
        if(howmuchtoprint < NUM_COLS)
            memset(temp_string+howmuchtoprint, 0, 80-howmuchtoprint);
		for(i = 0; i < howmuchtoprint; ++i)
            putc(temp_string[i]);
		file_size -= are_we_good;
		printed += are_we_good;
	}
    memcpy(becausenonullisguaranteed, to_print.file_name, FILENAME_LEN);
	printf("\n%s", becausenonullisguaranteed);
}
