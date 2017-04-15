#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"


#define PAGE_SIZE_4kB 4096 // number of bytes in a 4kB page

// malloc and free functions -- description in implementation
extern void * malloc(uint32_t size);
extern void free(void *ptr);


#endif
