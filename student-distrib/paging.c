#include <stdio.h>
#include "types.h"

// Bit masks to isolate portions of the virtual address.
#define TOP_10_BITS 0xFFC00000
#define MIDDLE_10_BITS 0x003FF000
#define LAST_12_BITS 0x00000FFF
#define ALL_BUT_LAST_12_BITS 0xFFFFF000
#define LAST_22_BITS 0x003FFFFF

uint32_t resolve_virt_addr(uint32_t);

int
main(){
    resolve_virt_addr(0);
    return 0;
}

void * resolve_virt_addr(void * virt_addr){
    uint32_t local_cr_3, return_addr;
    asm volatile ("movl %%cr3, %%eax; "
        "movl %%eax, %0;"
        : "=m"(local_cr_3)
        :
        : "%eax"
    );

    // Find entry in page directory
    uint32_t dir_index = virt_addr & TOP_10_BITS;
    dir_index >>= 22; // bit shift to get top 10 bits to lowest 10

    // Get address to page table/large page
    uint32_t dir_addr = (void *)(local_cr_3+4*dir_index);

    // Check if the page table/large page actually exists in memory
    if(!(dir_addr & 0x00000001))
        return NULL;
    
    // Check if this is page table or large page
    if(dir_addr & 0x00000080){
        // This is a 4MB page.
        uint32_t page_index = virt_addr & LAST_22_BITS;
        dir_addr &= ALL_BUT_LAST_12_BITS;
        dir_addr += page_index;
        return_addr = dir_addr;
    }
    else{
        // This is a page table. Find entry in page table
        uint32_t table_index = virt_addr & MIDDLE_10_BITS;
        table_index >>= 12;
        // Get address to small page
        dir_addr &= ALL_BUT_LAST_12_BITS;
        uint32_t table_addr = *(dir_addr+4*table_index);
        // Check if the page table/large page actually exists in memory
        if(!(table_addr & 0x00000001))
            return NULL;
        uint32_t page_index = virt_addr & LAST_12_BITS;
        table_addr &= ALL_BUT_LAST_12_BITS;
        table_addr += page_index;
        return_addr = table_addr;
    }
    return (void *)return_addr;
}

void
map_page(void * physaddr, void * virtualaddr, unsigned int flags){
{
    // Make sure that both addresses are page-aligned. 
    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
    unsigned long * pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.
 
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?
 
    pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01; // Present
 
    // Now you need to flush the entry in the TLB
    // or you might not notice the change.
}