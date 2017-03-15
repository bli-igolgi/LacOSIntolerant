#ifndef FILE_SYS_H
#define FILE_SYS_H

struct file_desc {
	uint32_t file_name[8],     // File name is 32 bytes
	uint32_t file_type,
	uint32_t inode_num,
	uint32_t reserved[6]       // Reserved section is 24 bytes
};

uint32_t file_write();

#endif
