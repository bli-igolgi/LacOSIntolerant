#include "paging.h"
#include "types.h"

// bit masks to isolate portions of the virtual address.
#define TOP_10_BITS 0xFFC00000                  // dirctory bits
#define MIDDLE_10_BITS 0x003FF000               // table bits
#define LAST_12_BITS 0x00000FFF                 // 12 offset bits for small offset
#define LAST_22_BITS 0x003FFFFF                 // last 22 bits for large offest
#define PRESENT_BIT 0x1                         // is page present?
#define RW_BIT 0x2                              // is read/write set?
#define US_BIT 0x4                              // is user/supervisor set?
#define PAGE_SIZE_BIT 0x80                      // is this a 4MB page or a 4KB page?

// bit mask to ignore the last 12 bits when examining the page directory or a page table
#define ALL_BUT_LAST_12_BITS 0xFFFFF000


void * resolve_virt_addr(void * virt_addr){
    uint32_t local_cr_3, return_addr;
    uint32_t dir_index, table_index, page_index;
    uint32_t dir_addr, table_addr; 

    // goal: store the contents of cr3 in local_cr_3
    asm volatile ("movl %%cr3, %%eax; "
        "movl %%eax, %0;"
        : "=m"(local_cr_3)
        :
        : "%eax"
    );

    // Compute the offset for the page directory -- bit shifting to get top 10 bits in lowest 10
    dir_index = (uint32_t)virt_addr & TOP_10_BITS;
    dir_index >>= 22;

    // Get the address of the page table or large page -- each entry is 4 bytes
    dir_addr = *(uint32_t *)(local_cr_3 + 4*dir_index);

    // Check if the page table/large page actually exists in memory -- check least significant bit
    if(!(dir_addr & PRESENT_BIT))
        return NULL;
    
    // Check if this is page table or large page
    if(dir_addr & PAGE_SIZE_BIT)
    {
        // 4MB page -- compute the physical address
        page_index = (uint32_t)virt_addr & LAST_22_BITS;
        return_addr = dir_addr & TOP_10_BITS;
        return_addr += page_index;
    } 
    else 
    {
        // page table -- find the correct entry in the page table

        // compute the offset of the page table entry -- bit shift to get the offset in the lowest 10 bits
        table_index = (uint32_t)virt_addr & MIDDLE_10_BITS;
        table_index >>= 12;

        // compute the address to the 4kb page
        dir_addr &= ALL_BUT_LAST_12_BITS;
        table_addr = *(uint32_t *)(dir_addr + 4*table_index);

        // Check if the page table/large page exists
        if(!(table_addr & PRESENT_BIT))
            return NULL;

        // compute the offset from the start of the page
        page_index = (uint32_t)virt_addr & LAST_12_BITS;

        // compute the physical address of the page
        return_addr = (table_addr & ALL_BUT_LAST_12_BITS) + page_index;
    }
    return (void *)return_addr;
}

void paging_init()
{
    // clear the page directory
    int index;
    for (index=0; index < NUM_ENTRIES; index++)
        page_directory[index] = 0;

    // map the kenal in the page directory -- large page, kernal privileges, read/write
    map_page((void *)KERNAL_ADDR, (void *)KERNAL_ADDR, 1, 1, 1);

    // map the video memory in the page directory -- small page, user privileges, read/write
    map_page((void *)VIDEO_ADDR, (void *)VIDEO_ADDR, 0, 0, 1);


    // setting the bits to enable paging
    asm volatile (
        "movl  %0, %%cr3;\n"           // store the address of the page directory in cr3
        "movl  %%cr4, %%edi;\n"        // set PSE flag in cr4 to enable 4Mb pages
        "orl   $0x10, %%edi;\n"
        "movl  %%edi, %%cr4;\n"
        "movl  %%cr0, %%edi;\n"  // set bit 31 of cr0 to enable paging
        "orl   $0x80000000, %%edi;\n"
        "movl  %%edi, %%cr0;\n"
        :
        : "r"(page_directory)
        : "%edi"
    );
}

// returns 0 on success, -1 on failure
int map_page(void * phys_addr, void * virtual_addr, int page_size, int privileges, int write)
{
    unsigned long page_dir_index, page_dir_entry, page_table_index, page_table_entry;
    unsigned long * page_dir_addr, * page_table_addr;

    // check for valid pointers
    if (phys_addr == NULL || virtual_addr == NULL)
        return -1;

    // goal: store the contents of cr3 in page_dir_addr
    asm volatile ("movl %%cr3, %%eax; "
        "movl %%eax, %0;"
        : "=m"(page_dir_addr)
        :
        : "%eax"
    );

    // Find appropriate page directory entry using index from virtual address
    page_dir_index = (unsigned long)virtual_addr >> 22;
    page_dir_entry = page_dir_addr[page_dir_index];

    // check if empty
    if(!(page_dir_entry & PRESENT_BIT)){
        // it's empty! set up the page/page table
        ;
    }
    // otherwise
    else{
        // it's not empty!
        // if not "want small page and page table there"
        if(!(page_size == 0 && !(page_dir_entry & PAGE_SIZE_BIT)))
            // fail
            return -1;
        // else keep going and check page table
        page_table_index = (unsigned long)virtual_addr >> 12 & 0x03FF;
        page_table_addr = (unsigned long *)(page_dir_entry & ALL_BUT_LAST_12_BITS);
        page_table_entry = page_table_addr[page_table_index];
        if(page_table_entry & PRESENT_BIT)
            return -1;
    }


    // Make sure that both addresses are page-aligned. 
 
/*    unsigned long * pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
    // When it is not present, you need to create a new empty PT and
    // adjust the PDE accordingly.
 
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
    // When it is, then there is already a mapping present. What do you do now?
 
    pt[ptindex] = ((unsigned long)physaddr) | (flags & 0xFFF) | 0x01; // Present
 
    // Now you need to flush the entry in the TLB
    // or you might not notice the change.


// */
    return 0;
}


// what guy next to me said:
// flash the tlb before you try paging
// the very last thing you do is to enable paging
