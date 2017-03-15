#ifndef FILE_SYS_H
#define FILE_SYS_H

struct file_desc {
	uint32_t file_name[8],     // File name is 32 bytes
	uint32_t file_type,
	uint32_t inode_num,
	uint32_t reserved[6]       // Reserved section is 24 bytes
};

uint32_t file_write();

// Reading from directory should read all file names and add to buffer, including '.'
// Reading from file should read all of the bytes and add to buffer, even non-printable characters
// inode0 is NULL

#endif
