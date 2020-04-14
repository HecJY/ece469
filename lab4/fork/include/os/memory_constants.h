#ifndef	_memory_constants_h_
#define	_memory_constants_h_

//------------------------------------------------
// #define's that you are given:
//------------------------------------------------

// We can read this address in I/O space to figure out how much memory
// is available on the system.
#define	DLX_MEMSIZE_ADDRESS	0xffff0000

// Return values for success and failure of functions
#define MEM_SUCCESS 1
#define MEM_FAIL -1

//--------------------------------------------------------
// Put your constant definitions related to memory here.
// Be sure to prepend any constant names with "MEM_" so 
// that the grader knows they are defined in this file.

//--------------------------------------------------------
#define MEM_L1FIELD_FIRST_BITNUM 12
//the max is the 2^8, and 12 for the page offset
#define MEM_MAX_VIRTUAL_ADDRESS 0xFFFFF
#define MEM_PTE_READONLY 0x4
#define MEM_PTE_DIRTY 0x2
#define MEM_PTE_VALID 0x1
#define MEM_PAGESIZE (0x1 << 12)
#define MEM_L1TABLE_SIZE (MEM_MAX_VIRTUAL_ADDRESS + 1) >> 12
#define MEM_ADDRESS_OFFSET_MASK (MEM_PAGESIZE - 1)
#define MEM_MAX_PHYS_MEM (0x1 << 21)
#define MEM_MAX_PAGES (MEM_MAX_PHYS_MEM / MEM_PAGESIZE)
#define MEM_PTE_MASK (~(MEM_PTE_READONLY | MEM_PTE_DIRTY | MEM_PTE_VALID))

// num_pages = size_of_memory / size_of_one_page
#define NUM_PAGES MEM_MAX_PAGES/32




#endif	// _memory_constants_h_
