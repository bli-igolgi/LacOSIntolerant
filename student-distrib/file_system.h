#ifndef FILE_SYS_H
#define FILE_SYS_H

#include "driver.h"

typedef struct dentry_t {
	uint32_t file_name[8];     // File name is 32 bytes
	uint32_t file_type;
	uint32_t inode_num;
	uint32_t reserved[6];      // Reserved section is 24 bytes
} dentry_t;

extern int32_t fsys_open(const uint8_t* filename);
extern int32_t fsys_close(int32_t fd);
extern int32_t fsys_read_file(int32_t fd, void *buf, int32_t nbytes);
extern int32_t fsys_read_dir(int32_t fd, void *buf, int32_t nbytes);
extern int32_t fsys_write(int32_t fd, const void *buf, int32_t nbytes);

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

// Reading from directory should read all file names and add to buffer, including '.'
// Reading from file should read all of the bytes and add to buffer, even non-printable characters
// inode0 is NULL

#endif
