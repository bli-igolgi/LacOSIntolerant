#include "paging.h"
#include "lib.h"
#include "types.h"
#include "paging.h"



/* 
 * malloc
 *   DESCRIPTION:  Allocates memory for a user program
 *                 Marks the memory as in use
 *   INPUTS:       size -- the number of bytes the user has requested
 *   RETURN VALUE: success -- a pointer to the memory allocated to the user program
 *                 failure -- NULL pointer
 *   SIDE EFFECTS: sets bits in the user's page table
 */
void * malloc(uint32_t size) {

	void * retval; // address of the memory the user requested

	// check for the amount of memory the user wants
	if (size >= PAGE_SIZE_4kB) {
		// call function to allocate multiple (or one) pages
		retval = malloc_large(size);
	} else {
		// call function to allocate a part of a page
		retval = malloc_small(size);
	}

	return retval;

}

	// convert the size the smallest power of two that is larger than the number
	uint32_t mem_size; // the size of the memory chunck we need to find
	uint32_t num_pages; // the number of pages we need to find

	// find an open page
	// check each page
	for(index=2; index<PAGE_ENTRIES+1; index++){

	}

	// if we could not find a page, return failure
	return NULL;

	// somehow mark it as present

	// change the user's page table and page directory bits

	// should we flush the tlb?


/* 
 * free
 *   DESCRIPTION:  Deallocates memory allocated with malloc
 *                 Marks the memory as no longer in use
 *   INPUTS:       ptr -- a pointer to the memory to be freed
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes bits in the user's page table
 */
void free(void *ptr){
	// check if the page is present

	// mark it as no longer present

	// change the user's page table and page directory bits

	// because the page table was changed, we might need to flush the tlb

	return;
}



/******** NOTE: init_heap() needs to be called during the initialization*****/

/*
 * init_heap
 *   DESCRIPTION: sets up the bookkeeping info for the heap
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIZE EFFECTS: creates a kernal page for the bookkeeping info
 */
void init_heap() {
	// get the address of the last page in memory
	uint32_t * page = SIZE_256MB - SIZE_4kB;

	// map the last 4kB page -- 4kB page, kernel privileges, read/write
	map_page(page, page, 0, 0, 1);

	// clear the last page (initialize bookkeeping info)
	for(i=0;i<PAGE_ENTRIES;i++)
		page[i] = 0;

	return;
}



// 256MB is the size of memory
// last page - bookkeeping - starts at 256MB - 4kB
// the start of pages range from 256MB - 2*4kB to 256MB - 1024*4kB



/* Basic implementation:
The user requests memory
As long as the memory is less than the size of a page, allocate it
Maximum size of heap is 1023 separate 4kB pages
Use the bottom page to mark if individual pages are in use
Use a loop; check the lowest page first and go upward
Return a pointer to the top of the first available page you find in that loop
If all 1023 pages are in use, return NULL
*/


/* More advanced implementation:
Divide a page into smaller memory chunks
Give the user the smallest chunk that is still larger than or equal to the memory he requested
At the top of the page, we have information about which pages are in use and how large they are


When the user requests a page, we check the free list first
If no chunks of the right size are available, we divide the next larger chunck in two
If there are no free blocks of a large enough size, we check the next page


This assumes that the user will only write the exact amount of bytes he requested

*For this implementation, I need to decide on the basic unit of memory
*/
