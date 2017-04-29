#include "malloc.h"



/* 
 * malloc
 *   DESCRIPTION:  Allocates memory for a user program
 *                 Marks the memory as in use
 * 				   Calls the appropriate helper function based on the page size
 *   INPUTS:       size -- the number of bytes the user has requested
 *   RETURN VALUE: success -- a pointer to the memory allocated to the user program
 *                 failure -- NULL pointer
 *   SIDE EFFECTS: sets bits in the page tables
 */
void * malloc(uint32_t size) {

	void * retval; // address of the memory the user requested


	// check for the amount of memory the user wants
	if (size > PAGE_SIZE_4kB) {
		// return NULL becaue this is not currently supported
		retval = NULL;
	} else {
		// call function to allocate all or part of a page
		retval = malloc_small(size);
	}

	return retval;
}


void * malloc_small(uint32_t size) {

	uint32_t mem_size; // the size of the memory chunck to allocate
	uint32_t * page_start; // address of the start of the page we are considering
	uint32_t * bookkeeping; // address of the bookkeeping info
	void * ret_val;
	uint32_t index; // counter for the for loop



	// convert the size to the smallest size we can allocate larger than that number
	if (size <= 256) {
		mem_size = 256;
	} else if (size <= 512) {
		mem_size = 512;
	} else if (size <= 1024) {
		mem_size = 1024;
	} else if (size <= 2048) {
		mem_size = 2048;
	} else {
		mem_size = 4096;
	}

	// search through the pages for a page with an open spot
	for (index=0; index<PAGE_ENTRIES - 1; index++) {
		// compute the address of the start of the page
		page_start = (uint32_t *)(MALLOC_FIRST_ADDR + PAGE_SIZE_4kB * index);

		// compute the address of the bookkeeping entry
		bookkeeping = (uint32_t *) (MALLOC_BOOK + MALLOC_BOOK_ENTRY * index);

		// if the page has not been mapped, map it and set up the bookkeeping info
		if ((*bookkeeping & MALLOC_PRESENT) == 0) {
			// map the 4kB page with user privileges and read/write
			map_page(page_start, page_start, 0, 1, 1, 1);

			// set the "present bit"
			*bookkeeping  = MALLOC_PRESENT;
		}

		// check for the correct size chunk of memory

		// check for the correct size
		// if the correct size is not available, check for the next larger size
		


		// return the a pointer to this memory
		return ret_val;

	}


	// return failure if we could not find a page
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
void free(void *ptr){

	uint32_t index; // index of the bookkeeping info
	uint32_t * bookkeeping; // address at which bookkeeping info is stored
	uint32_t memory_index; // index of the memory chunk
	uint32_t bitmask; // bitmask to check for specific present bits in the bookkeeping info

	/* check for a valid address */
	// must be 8 trailing zeros
	if (((uint32_t)ptr & MALLOC_TRAILING_8) != 0)
		return;

	// make sure the address is in the valid range
	if ((uint32_t)ptr < MALLOC_FIRST_ADDR || (uint32_t)ptr > MALLOC_LAST_ADDR)
		return;

	/* compute the index of the entry in the bookkeeping info - same as number of the 4kb page */
	// discard the upper bits and the lower 12 bits
	index = ((uint32_t)ptr - MALLOC_FIRST_ADDR) >> 12;

	/********** TEST POINT: make sure this is always a valid index **********/

	// get the address at which bookkeeping info is stored
	bookkeeping = (uint32_t *)(MALLOC_BOOK + index);

	// check the present bit -- return if not present
	if ((*bookkeeping & MALLOC_PRESENT) == 0)
		return;

	/* compute the "index" of the page */
	// discard the upper 20 bits and the lower 8 bits
	memory_index = (((uint32_t)ptr & MALLOC_TRAILING_12) >> 8);


	/*********** SHOULD USE A DO WHILE LOOP HERE ***********/


	// check for a 256 byte page
	bitmask = MALLOC_256 >> memory_index;
	if ((bitmask & *bookkeeping) == 1) {
		/* found the correct page */
		clear_bit(bookkeeping, memory_index, 256);
		return;
	}


	/****** check for 512 byte page ******/
	// check for invalid page alignment
	if ((memory_index % 2) != 0)
		return;

	// readjust the memory index for 512 and recompute the bitmask
	memory_index = memory_index >> 1;
	bitmask = MALLOC_512 >> memory_index;

	if ((bitmask & *bookkeeping) == 1) {
		/* found the correct page */
		clear_bit(bookkeeping, memory_index, 512);
		return;
	}


	/****** check for 1024 byte page ******/
	// check for invalid page alignment
	if ((memory_index % 2) != 0)
		return;

	// readjust the memory index for 1024 and recompute the bitmask
	memory_index = memory_index >> 1;
	bitmask = MALLOC_1024 >> memory_index;

	if ((bitmask & *bookkeeping) == 1) {
		/* found the correct page */
		clear_bit(bookkeeping, memory_index, 1024);
		return;
	}


	/****** check for 2048 byte page ******/
	// check for invalid page alignment
	if ((memory_index % 2) != 0)
		return;

	// readjust the memory index for 2048 and recompute the bitmask
	memory_index = memory_index >> 1;
	bitmask = MALLOC_2048 >> memory_index;

	if ((bitmask & *bookkeeping) == 1) {
		/* found the correct page */
		clear_bit(bookkeeping, memory_index, 2048);
		return;
	}


	/****** check for 4096 byte page ******/
	// check for invalid page alignment
	if ((memory_index % 2) != 0)
		return;

	// readjust the memory index for 2048 and recompute the bitmask
	memory_index = memory_index >> 1;
	bitmask = MALLOC_4096 >> memory_index;

	if ((bitmask & *bookkeeping) == 1) {
		/* found the correct page */
		clear_bit(bookkeeping, memory_index, 4096);
		return;
	}

	return;
}


void clear_bit(uint32_t * bkkp_entry, uint32_t mem_index, uint32_t type)
{
	uint32_t sibling_index; // index to the sibling bit
	uint32_t sibling_bitmask; // bitmask for the sibling
	uint32_t bitmask; // bitmask for the bit we are considering

	switch(type) {
		case 256: {
			// compute the bitmask
			bitmask = MALLOC_256 >> mem_index;

			// set the bit to zero
			*bkkp_entry = *bkkp_entry & (~bitmask);

			// get the index to the sibling bit - flip the LSB
			sibling_index = mem_index ^ 0x1;
			sibling_bitmask = MALLOC_256 >> sibling_index;

			// check if the sibling is in use -- if not, keep going
			if ((sibling_bitmask & *bkkp_entry) != 0)
				break;

			// shift the index for the next level
			mem_index = mem_index >> 1;
		}

		case 512: {
			// compute the bitmask
			bitmask = MALLOC_512 >> mem_index;

			// set the bit to zero
			*bkkp_entry = *bkkp_entry & (~bitmask);

			// get the index to the sibling bit - flip the LSB
			sibling_index = mem_index ^ 0x1;
			sibling_bitmask = MALLOC_512 >> sibling_index;

			// check if the sibling is in use -- if not, keep going
			if ((sibling_bitmask & *bkkp_entry) != 0)
				break;

			// shift the index for the next level
			mem_index = mem_index >> 1;
		}

		case 1024: {
			// compute the bitmask
			bitmask = MALLOC_1024 >> mem_index;

			// set the bit to zero
			*bkkp_entry = *bkkp_entry & (~bitmask);

			// get the index to the sibling bit - flip the LSB
			sibling_index = mem_index ^ 0x1;
			sibling_bitmask = MALLOC_1024 >> sibling_index;

			// check if the sibling is in use -- if not, keep going
			if ((sibling_bitmask & *bkkp_entry) != 0)
				break;

			// shift the index for the next level
			mem_index = mem_index >> 1;
		}

		case 2048: {
			// compute the bitmask
			bitmask = MALLOC_2048 >> mem_index;

			// set the bit to zero
			*bkkp_entry = *bkkp_entry & (~bitmask);

			// get the index to the sibling bit - flip the LSB
			sibling_index = mem_index ^ 0x1;
			sibling_bitmask = MALLOC_2048 >> sibling_index;

			// check if the sibling is in use -- if not, keep going
			if ((sibling_bitmask & *bkkp_entry) != 0)
				break;

			// shift the index for the next level
			mem_index = mem_index >> 1;
		}

		case 4096: {
			// compute the bitmask
			bitmask = MALLOC_4096 >> mem_index;

			// set the bit to zero
			*bkkp_entry = *bkkp_entry & (~bitmask);

			break;
		}

		default:
			break;
	}
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
	uint32_t * page = (uint32_t *)MALLOC_BOOK;
	uint32_t i; // counter for for loop

	// map the last 4kB page -- 4kB page, kernel privileges, read/write
	map_page(page, page, 0, 0, 1, 1);

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
