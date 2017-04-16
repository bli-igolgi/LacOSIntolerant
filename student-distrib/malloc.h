#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"


#define PAGE_ENTRIES 1024   // number of unsigned longs in a 4kB page
#define PAGE_SIZE_4kB 4096  // number of bytes in a 4kB page
#define MEM_SIZE 0x10000000 // the size of the memory space

#define MALLOC_PRESENT 0x80000000 // bitmask for present (most significant) bit in bookkeeping info
#define MALLOC_4096    0x40000000 // bitmask for the 4096 block present bit
#define MALLOC_2048
#define MALLOC_1024
#define MALLOC_512
#define MALLOC_256


// malloc and free functions -- description in implementation
extern void * malloc(uint32_t size);
extern void * malloc_small(size);
extern void free(void *ptr);
extern void init_heap();


#endif
