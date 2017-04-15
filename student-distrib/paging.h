#ifndef PAGING_H
#define PAGING_H

#include "types.h"


/************ constansts used for paging ************/
#define SIZEOFDIR   4096              // size of small page, page directory, page table.... (4kB)
#define NUM_ENTRIES 1024              // the number of entries in a page table or page directory

#define KERNEL_ADDR 0x400000          // address of the kernal in physical memory
#define VIDEO       0xB8000           // start of video memory, from lib.c
#define PAGE_TABLE_STARTADDR 0x8800000 // start address of the page table

// bit masks to isolate portions of the virtual address/page table or directory entries
#define TOP_10_BITS     0xFFC00000          // dirctory bits
#define MIDDLE_10_BITS  0x003FF000          // table bits
#define LAST_12_BITS    0x00000FFF          // 12 offset bits for small offset
#define LAST_10_BITS    0x03FF              // needed after shifting by 12 to isolate table entry number
#define LAST_22_BITS    0x003FFFFF          // last 22 bits for large offest
#define ALL_BUT_LAST_12_BITS 0xFFFFF000     // ignore the last 12 bits in the page directory or a page table

// information bit masks for page table/directory entries - 1 means yes
#define PRESENT_BIT   0x1             // is page present?
#define RW_BIT        0x2             // is read/write set?
#define US_BIT        0x4             // is supervisor set?
#define PAGE_SIZE_BIT 0x80            // is this a 4MB page?

#define SHIFT_FOR_DIRENTRY   22           // number of bits to remove to isolate directory entry number
#define SHIFT_FOR_TABLEENTRY 12           // number of bits to remove to isolate table entry number
/****************************************************/


// global variable for the page directory
uint32_t page_directory[NUM_ENTRIES] __attribute__((aligned (SIZEOFDIR)));


// paging functions -- function descriptions at the implementations
extern void * resolve_virt_addr(void * virt_addr);
extern void paging_init();
extern int map_page(void * phys_addr, void * virtual_addr, bool page_size, bool privileges, bool write);

#endif

