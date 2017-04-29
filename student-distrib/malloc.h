#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"
#include "paging.h"
#include "lib.h"


#define PAGE_ENTRIES 1024    // number of unsigned longs in a 4kB page
#define PAGE_SIZE_4kB 4096   // number of bytes in a 4kB page
#define MEM_SIZE 0x100000000 // the size of the memory space

#define MALLOC_PRESENT 0x80000000 // bitmask for present (most significant) bit in bookkeeping info
#define MALLOC_4096    0x40000000 // bitmask for the 4096 block present bit
#define MALLOC_2048    0x20000000 // bitmask for the first 2048 present bit
#define MALLOC_1024    0x04000000 // bitmask for the first 1024 present bit
#define MALLOC_512     0x00400000 // bitmask for the first 512 present bit
#define MALLOC_256     0x00004000 // bitmask for the first 256 present bit

#define MALLOC_TRAILING_8  0x0ff // bitmask for the last 8 bits
#define MALLOC_TRAILING_12 0xfff // bitmask for the last 12 bits

#define MALLOC_FIRST_ADDR 0xffc00000 // address of the start of the first address space
#define MALLOC_LAST_ADDR  0xffffe000 // address of the start of the last address space
#define MALLOC_BOOK       0xfffff000 // address of the start of bookkeeping info

#define MALLOC_BOOK_ENTRY 0x04       // size of a bookkeeping entry
#define NUM_256 256 // the number 256

// malloc and free functions -- description in implementation
extern void * malloc(uint32_t size);
extern void * malloc_small(uint32_t size);
extern void free(void *ptr);
extern void init_heap();
extern void clear_bit(uint32_t * bkkp_entry, uint32_t mem_index, uint32_t type);


#endif
