#include "paging.h"
#include "lib.h"
#include "types.h"



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
	// if the user requests too many bytes (more than one 4kB page), return NULL
	if(size > PAGE_SIZE_4kB)
		return NULL;

	// find an open page

	// somehow mark it as present

	// change the user's page table and page directory bits

	// should we flush the tlb?


	return NULL;
}


/* 
 * free
 *   DESCRIPTION:  Deallocates memory allocated with malloc
 *                 Marks the memory as no longer in use
 *   INPUTS:       ptr -- a pointer to the memory to be freed
 *   RETURN VALUE: none
 *   SIDE EFFECTS: changes bits in the user's page table
 */
extern void free(void *ptr){
	// check if the page is present

	// mark it as no longer present

	// change the user's page table and page directory bits

	// because the page table was changed, we might need to flush the tlb

	return;
}

/*
 * init_heap
 *   DESCRIPTION: sets up the bookkeeping info for the heap
 *   INPUTS: none
 *   RETURN VALUE: none
 *   SIZE EFFECTS: creates a kernal page for the bookkeeping info
 */


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
