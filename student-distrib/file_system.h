#ifndef FILE_SYS_H
#define FILE_SYS_H

#include "driver.h"
#define RESERVED_BOOT 13 // number of reserved bytes in boot block / 4
#define RESERVED_USER 6  // number of reserved bytes in inodes / 4
#define FILENAME_LEN 32  // length of filename in bytes
#define ENTRY_SIZE_BYTES 64	// size of each entry (bytes)
#define ENTRY_SIZE_UINTS 16 // size of each entry (integer array indices)
#define BLK_SIZE_BYTES 4096 // size of block (bytes)
#define BLK_SIZE_UINTS 1024 // size of block (integer array indices)

uint32_t * fs_addr; // on the off-chance that the filesystem is not found at the
                      // same addr each time; this is set in kernel.c

typedef struct dentry_t {
	char file_name[32];     // File name is 32 bytes
	uint32_t file_type;
	uint32_t inode_num;
	uint32_t reserved[6];      // Reserved section is 24 bytes
} dentry_t;

typedef struct fdesc_t {
	uint32_t file_ops;
	uint32_t inode;
	uint32_t file_position;
	uint32_t flags;
} fdesc_t;

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

void test_access_by_index(){
	dentry_t to_print;
	char becausenonullisguaranteed[33];
	becausenonullisguaranteed[32] = '\0';
	uint32_t num_of_files = fs_addr[0];
   	int i, file_size;
   	for(i=0;i<num_of_files;++i){
		read_dentry_by_index(i, &to_print);
		strncpy(becausenonullisguaranteed, to_print.file_name, FILENAME_LEN);
		printf("%s, %u, ", becausenonullisguaranteed, to_print.file_type);
		file_size = fs_addr[(i+1)*ENTRY_SIZE_UINTS+FILENAME_LEN/4]; // over 4 because uint32_t indices
		printf("%u\n", file_size);
	}
}

void test_access_by_file_name(){
	char* filename = "frame0.txt";
	dentry_t to_print;
	char* temp_string[80];
	uint32_t file_size, printed = 0;
	read_dentry_by_name(filename, &to_print);
	file_size = fs_addr[(to_print.inode_num+1)*ENTRY_SIZE_UINTS+FILENAME_LEN/4] // over 4 because uint32_t indices
	while(file_size > 0){
		// this is pretty dreadful but we don't have malloc yet
		read_data(to_print.inode_num, printed, temp_string, (file_size>=80)?80:file_size);
		printf("%s", temp_string);
		file_size -= 80;
		printed += 80;
	}
	printf("\n%s", to_print.file_name);
}

void test_data_printing(){
	static dentry_t to_print;
	static int num_of_files = fs_addr[0];
	static int index_to_be_printed = num_of_files-1;
	uint32_t file_size, printed = 0;
	index_to_be_printed = (index_to_be_printed++)%num_of_files;
	read_dentry_by_index(index_to_be_printed, to_print);
	file_size = fs_addr[(to_print.inode_num+1)*ENTRY_SIZE_UINTS+FILENAME_LEN/4];
	while(file_size > 0){
		// this is pretty dreadful but we don't have malloc yet
		read_data(to_print.inode_num, printed, temp_string, (file_size>=80)?80:file_size);
		printf("%s", temp_string);
		file_size -= 80;
		printed += 80;
	}
	printf("\n%s", to_print.file_name);	
}

#endif
