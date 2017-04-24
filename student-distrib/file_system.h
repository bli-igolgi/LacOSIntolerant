#ifndef FILE_SYS_H
#define FILE_SYS_H

#include "driver.h"
#define RESERVED_BOOT       13      // number of reserved bytes in boot block / 4
#define RESERVED_USER       6       // number of reserved bytes in inodes / 4
#define FILENAME_LEN        32      // length of filename in bytes
#define ENTRY_SIZE_BYTES    64	    // size of each entry (bytes)
#define ENTRY_SIZE_UINTS    16      // size of each entry (integer array indices)
#define BLK_SIZE_BYTES      4096    // size of block (bytes)
#define BLK_SIZE_UINTS      1024    // size of block (integer array indices)

uint32_t * fs_addr; // on the off-chance that the filesystem is not found at the
                      // same addr each time; this is set in kernel.c

typedef struct dentry_t {
	char file_name[32];     // File name is 32 bytes
	uint32_t file_type;
	uint32_t inode_num;
	uint32_t reserved[6];      // Reserved section is 24 bytes
} dentry_t;


extern int32_t fsys_open_file(const uint8_t* filename);
extern int32_t fsys_close_file(int32_t fd);
extern int32_t fsys_open_dir(const uint8_t* filename);
extern int32_t fsys_close_dir(int32_t fd);
extern int32_t fsys_read_file(int32_t fd, void *buf, int32_t nbytes);
extern int32_t fsys_read_dir(int32_t fd, void *buf, int32_t nbytes);
extern int32_t fsys_write_file(int32_t fd, const void *buf, int32_t nbytes);
extern int32_t fsys_write_dir(int32_t fd, const void *buf, int32_t nbytes);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

void test_access_by_index();
void test_access_by_file_name();
void test_data_printing();
// Reading from directory should read all file names and add to buffer, including '.'
// Reading from file should read all of the bytes and add to buffer, even non-printable characters
// inode0 is NULL
extern struct f_ops_table dir_jt;
extern struct f_ops_table regf_jt;

#endif
