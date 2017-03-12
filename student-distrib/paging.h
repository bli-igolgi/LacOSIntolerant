#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define PAGE_TABLE_STARTADDR 0x800000
#define SIZEOFDIR 4096       // size of small page, page directory, page table.... (4kB)
#define NUM_ENTRIES 1024     // the number of entries in a page table or page directory
#define KERNAL_ADDR 0x400000   // address of the kernal in physical memory

// global variable for the page directory
uint32_t page_directory[NUM_ENTRIES] __attribute__((aligned (SIZEOFDIR)));


// functions related to paging
extern void * resolve_virt_addr(void * virt_addr);
extern void paging_init();
extern int map_page(void * phys_addr, void * virtual_addr, int page_size, int privileges, int write);

#endif

