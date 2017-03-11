#ifndef PAGING_H
#define PAGING_H

#include "types.h"


#define NUM_ENTRIES 1024     // the number of entries in a page table or page directory
#define KERNAL_ADDR 0x8000   // address of the kernal in physical memory
#define VIDEO_ADDR 0         // address of the video memory in physical memory

// global variable for the page directory
int32_t page_directory[NUM_ENTRIES] __attribute__((aligned (4096)));


// functions related to paging
extern void * resolve_virt_addr(void * virt_addr);
extern void paging_init();
extern int map_page(void * phys_addr, void * virtual_addr, int page_size, int privileges, int write);


#include "paging.c"

#endif

