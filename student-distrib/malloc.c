#include "malloc.h"



// bitmasks for the first present bit in each type of page block
uint32_t first_present_bit[5] = {
	0x40000000, // 4096
	0x20000000, // 2048
	0x04000000, // 1024
	0x00400000, // 512
	0x00004000  // 256
};

// number of sections of memory available per 4kB page
uint32_t NUM_AVAIL[5] = {
	1, // size is 4096 
	2, // size is 2048
	4, // size is 1024
	8, // size is 512
	16 // size is 256
};


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
void * malloc(uint32_t size)
{

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


/* 
 * malloc_small
 *   DESCRIPTION:  finds memory at least as big as the user requested for the user to have
 *                 fills out the page table if necessary
 *                 marks the memory as "in use" in the bookkeeping info
 *   INPUTS:       the size of the memory the user wants
 *   RETURN VALUE: the pointer to the memory we allocated
 *   SIDE EFFECTS: may map a page in the page tables
 *                 sets "in use" bits in the bookkeeping info
 */
void * malloc_small(uint32_t size)
{

	uint32_t mem_size_type; // type of the size of memory to be allocated
	uint32_t * page_start; // address of the start of the page we are considering
	uint32_t * bookkeeping; // address of the bookkeeping info
	uint32_t index; // counter for the for loop
	uint32_t mem_size; // actual size of the memory

	// choose a type of memory size (described in malloc.h) based on the number of bytes we need
	if (size <= 256) {
		mem_size_type = 4;
	} else if (size <= 512) {
		mem_size_type = 3;
	} else if (size <= 1024) {
		mem_size_type = 2;
	} else if (size <= 2048) {
		mem_size_type = 1;
	} else {
		mem_size_type = 0;
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

		// check the availability of each address
		for (index=0; index < NUM_AVAIL[mem_size_type]; index++) {
			if (check_availability(bookkeeping, index, mem_size_type) == 0) {
				// if it is available, set the bits and return the address
				set_availability(bookkeeping, index, mem_size_type);
				mem_size = PAGE_SIZE_4kB >> mem_size_type;
				return (uint32_t *) (MALLOC_FIRST_ADDR + mem_size * index);
			}
		}

	}

	// return failure if we could not find a page
	return NULL;
}



/* 
 * check_availability
 *   DESCRIPTION:  checks if memory of a given size at a given position is available
 *   INPUTS:       bkkp_entry - the address of the bookkeeping long we are modifying
 *                 mem_index - index of the memory we are seeing if available
 *                 type - the size of the memory we want (types explained in malloc.h)
 *   RETURN VALUE: 0 if memory is available, -1 if memory is not available
 *   SIDE EFFECTS: none
 */
uint32_t check_availability(uint32_t * bkkp_entry, uint32_t mem_index, uint32_t type)
{
	uint32_t bitmask; // bitmask to check if a chunck of memory is in use
	uint32_t sibling_index; // index of the sibling
	uint32_t sibling_bitmask; // bitmask to check if sibling is in use

	// if we are checking the 4kB page, we just return success or failure
	if (type == 0) {
		if ((*bkkp_entry & first_present_bit[type]) == 0) {
			return SUCCESS;
		} else {
			return FAILURE;
		}
	}

	// if the page is marked in use, it is not available
	bitmask = (first_present_bit[type]) >> mem_index;
	if ((*bkkp_entry & bitmask) != 0)
		return FAILURE;

	// compute the index and the bitmask of the sibling
	sibling_index = mem_index ^ 0x1;
	sibling_bitmask = (first_present_bit[type]) >> sibling_index;

	// if the page is not in use, but its sibling is in use, we can use it
	if ((*bkkp_entry & sibling_bitmask) != 0)
		return SUCCESS;

	// otherwise, we need to check the availability of the parent
	return check_availability(bkkp_entry, mem_index > 1, type - 1);
}



/* 
 * set_availability
 *   DESCRIPTION:  sets the bits necessary to use memory with a given size at a given index
 *   INPUTS:       bkkp_entry - a pointer to the bookkeeping info
 *                 mem_index - the index at which the memory we want to use is at
 *                 type - the size of the page (types specified in malloc.h)
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets bits in the bookkeeping info
 */
void set_availability(uint32_t * bkkp_entry, uint32_t mem_index, uint32_t type)
{
	uint32_t bitmask; // bitmask to check if memory is in use

	// set the "in use" bits all the way up
	while (type >= 0) {
		// set the "in use" bit
		bitmask = (first_present_bit[type]) >> mem_index;
		*bkkp_entry = *bkkp_entry | bitmask;

		// remove the least significant bit and decrement the type
		mem_index = mem_index >> 1;
		type--;
	}
	return;
}



	// check for the correct size chunk of memory
	// if they are all present, we continue through the for loop
	// if they are not all present:
		// if parent and sibling are 1, we can use it
		// if parent is 1 and sibling is not, we can't use it

		// if parent is 0, we need to keep checking upwards


/*
// return NULL pointer for failure and a pointer for success
uint32_t * try1024(){
	uint32_t index;

	// check each page for an open one of the correct size -- exactly one sibling is occupied
	for(index=0; index < 4; index++){
		// compute the bitmask and the sibling's bitmask
		bitmask = MALLOC_1024 >> index;
		sibling_bitmask = MALLOC_1024 >> (index ^ 0x1);

		// if index is not is use, but sibling is, we can use index
		if((*bookkeeping & bitmask) == 0 && (*bookkeeping & sibling_bitmask) != 0) {
			// set the bit and return the address
			*bookkeeping = *bookkeeping | bitmask;
			return NULL; // NEED TO RETURN AN ADDRESS HERE
		}

	}

	// at this point there we no easy allocations, so we check parents for openings
	for(index=0; index < 4; index = index + 2) {
		// if this returns then, the parent's bit was set and we are free to use one of the siblings
		if (try2048 != NULL) {

		}
	}

}
*/


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
	if ((uint32_t)ptr < MALLOC_FIRST_ADDR || (uint32_t)ptr >= MALLOC_BOOK)
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
	for(i=0;i<(uint32_t)PAGE_ENTRIES;i++) {
		page[i] = 0x0;
	}

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
*/
