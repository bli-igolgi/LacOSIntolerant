#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"


#define PAGE_ENTRIES 1024   // number of unsigned longs in a 4kB page
#define PAGE_SIZE_4kB 4096  // number of bytes in a 4kB page
#define PAGE_SIZE_256MB 0x10000000 // the size of the memory space



// malloc and free functions -- description in implementation
extern void * malloc(uint32_t size);
extern void * malloc_small(size);
extern void free(void *ptr);
extern void init_heap();


#endif
