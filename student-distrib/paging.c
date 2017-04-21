#include "paging.h"
#include "lib.h"
#include "types.h"



/* 
 * resolve_virt_addr
 *   DESCRIPTION: Converts a virtual address into a physical address
 *                NOTE: this function is currently unused, but we expect to need to do this in a later checkpoint
 *   INPUTS: virt_addr -- a virtual address to be converted to a physical address
 *   RETURN VALUE: the physical address of the virtual address passed in
 *   SIDE EFFECTS: none
 */
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
    dir_index >>= SHIFT_FOR_DIRENTRY;

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
        table_index >>= SHIFT_FOR_TABLEENTRY;

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


/* 
 * paging_init()
 *   DESCRIPTION: maps the kernel to virtual memory; maps the video memory to virtual memory
 *                enables paging
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables virtual memory
 */
void paging_init()
{
    // clear the page directory
    int index;
    for (index=0; index < NUM_ENTRIES; index++)
        page_directory[index] = 0;

    // store the address of the page directory in cr3
    asm volatile (
        "movl  %0, %%cr3;\n"
        :
        : "r"(page_directory)
    );

    // map the kernel in the page directory -- large page, kernel privileges, read/write
    map_page((void *)KERNEL_ADDR, (void *)KERNEL_ADDR, 1, 0, 1, 0);

    // map the video memory in the page directory -- small page, kernel privileges, read/write
    map_page((void *)VIDEO, (void *)VIDEO, 0, 0, 1, 0);






    // map the alternate video memory page
    // map_page(*screen_start, VIDMAP_VIRT_ADDR, false, true, true, false);






    map_page((void *)PAGE_TABLE_STARTADDR, (void *)PAGE_TABLE_STARTADDR, 1, 0, 1, 0);

    // setting the bits to enable paging
    asm volatile (
        // set PSE flag in cr4 to enable 4MB pages
        "movl  %%cr4, %%edi;\n"
        "orl   $0x90, %%edi;\n"
        "movl  %%edi, %%cr4;\n"

        // set bit 31 of cr0 to enable paging
        "movl  %%cr0, %%edi;\n"
        "orl   $0x80000000, %%edi;\n"
        "movl  %%edi, %%cr0;\n"
        :
        : 
        : "%edi"
    );
}



/* 
 * map_page
 *   DESCRIPTION: maps a page at the physical address to the given virtual address
 *   INPUTS: phys_addr      -- the physical address to be mapped to
 *           virtual_addr   -- the virtual address given
 *           page_size      -- 1 for 4MB page, 0 for 4kB page
 *           privileges     -- 1 for user privileges, 0 for kernel privileges
 *           write          -- 1 for read/write, 0 for read only
 *           remap          -- used only for sys_execute, to overwrite mapping a page
 *   RETURN VALUE: 0 for a success, -1 for failure
 *   SIDE EFFECTS: modifies the page directory and/or a page table
 *                 may create a new page table
 */
int map_page(void * phys_addr, void * virtual_addr, bool page_size, 
                bool privileges, bool write, bool remap) {
    unsigned long page_dir_index, page_dir_entry, page_table_index, page_table_entry;
    unsigned long * page_dir_addr, * page_table_addr;
    int index;

    // check for valid pointers
    if (phys_addr == NULL || virtual_addr == NULL)
        return -1;

    // store the contents of cr3 in page_dir_addr
    asm volatile ("movl %%cr3, %%eax; "
        "movl %%eax, %0;"
        : "=m"(page_dir_addr)
        :
        : "%eax"
    );

    // Find appropriate page directory entry using index from virtual address
    page_dir_index = (unsigned long)virtual_addr >> SHIFT_FOR_DIRENTRY;
    page_dir_entry = page_dir_addr[page_dir_index];

    // check if the page directory entry is empty
    if(!(page_dir_entry & PRESENT_BIT) || remap){
        // it's empty! set up the page/page table

        // this is a 4MB page
        if(page_size == 1){
            // store physical address in page directory entry
            page_dir_entry = (unsigned long)phys_addr & ALL_BUT_LAST_12_BITS;

            // set the bits for large page, user/superuser, read/write, and present
            page_dir_entry += PAGE_SIZE_BIT;
            page_dir_entry += privileges*US_BIT;
            page_dir_entry += write*RW_BIT;
            page_dir_entry += PRESENT_BIT;
            // page_dir_entry += ((page_size&&(!privileges)&&write)?GLOBAL_BIT:0);

            // save this entry to the page directory
            page_dir_addr[page_dir_index] = page_dir_entry;
        }
        // this is a 4kB page
        else{
            // get the address to be stored in the page directory
            page_dir_entry = PAGE_TABLE_STARTADDR + SIZEOFDIR*page_dir_index;

            // to make lines 172+ easier, set the base address of the new page table
            page_table_addr = (unsigned long *)page_dir_entry;
            page_table_index = (unsigned long)virtual_addr >> SHIFT_FOR_TABLEENTRY & LAST_10_BITS;

            // initialize all page table entries
            for(index=0;index<NUM_ENTRIES;++index)
                page_table_addr[index] = 0;
            page_table_entry = page_table_addr[page_table_index];

            // set the bits for user/superuser, read/write, and present
            page_dir_entry += privileges*US_BIT;
            page_dir_entry += write*RW_BIT;
            page_dir_entry += PRESENT_BIT;

            // save this entry in the page directory
            page_dir_addr[page_dir_index] = page_dir_entry;
        }
    }
    // otherwise, it's not empty
    else{

        // if we want a small page but there is already a large page there, fail
        if(!(page_size == 0 && !(page_dir_entry & PAGE_SIZE_BIT)))
            return -1;

        // else keep going and check page table
        page_table_index = (unsigned long)virtual_addr >> SHIFT_FOR_TABLEENTRY & LAST_10_BITS;
        page_table_addr = (unsigned long *)(page_dir_entry & ALL_BUT_LAST_12_BITS);
        page_table_entry = page_table_addr[page_table_index];

        // if the entry we want in the page table is already used, fail
        if(page_table_entry & PRESENT_BIT)
            return -1;
    }

    if(page_size == 0){
        // get the address to be stored in the page table
        page_table_entry = (unsigned long)phys_addr & ALL_BUT_LAST_12_BITS;

        // set the bits for user/superuser, read/write, and present
        page_table_entry += privileges*US_BIT;
        page_table_entry += write*RW_BIT;
        page_table_entry += PRESENT_BIT;

        // save this entry in the page table
        page_table_addr[page_table_index] = page_table_entry;
    }

    return 0;
}



