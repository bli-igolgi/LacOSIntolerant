
// bit masks to isolate portions of the virtual address.
#define TOP_10_BITS 0xFFC00000                  // dirctory bits
#define MIDDLE_10_BITS 0x003FF000               // table bits
#define LAST_12_BITS 0x00000FFF                 // 12 offset bits for small offset
#define LAST_22_BITS 0x003FFFFF                 // last 22 bits for large offest

// bit mask to ignore the last 12 bits when examining the page directory or a page table
#define ALL_BUT_LAST_12_BITS 0xFFFFF000
#define ALL_BUT_LAST_22_BITS 0xFFC00000


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
    if(!(dir_addr & 0x00000001))
        return NULL;
    
    // Check if this is page table or large page
    if(dir_addr & 0x00000080)
    {
        // 4MB page -- compute the physical address
        page_index = (uint32_t)virt_addr & LAST_22_BITS;
        return_addr = dir_addr & ALL_BUT_LAST_22_BITS;
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
        if(!(table_addr & 0x00000001))
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

    // map the kenal in the page directory
    map_page((void *)KERNAL_ADDR, (void *)KERNAL_ADDR, 0); // NEED TO CHANGE THE PARAMETERS

    // map the video memory in the page directory
    map_page((void *)VIDEO_ADDR, (void *)VIDEO_ADDR, 0); // NEED TO CHANGE THE PARAMETERS


    // setting the bits to enable paging
    asm volatile (
        "movl  %0, %%cr3;\n"           // store the address of the page directory in cr3
        "orl   $0x10, %%cr4;\n"        // set PSE flag in cr4 to enable 4Mb pages
        "orl   $0x80000000, %%cr0;\n"  // set bit 31 of cr0 to enable paging
        :
        : "r"(page_directory)
    );
}


void map_page(void * physaddr, void * virtualaddr, unsigned int flags)
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


// what guy next to me said:
// flash the tlb before you try paging
// the very last thing you do is to enable paging
