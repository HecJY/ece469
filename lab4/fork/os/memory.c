//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

// num_pages = size_of_memory / size_of_one_page
static uint32 freemap[NUM_PAGES];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;
//*****Q3 Yuan
static int page_refcounters[MEM_MAX_PAGES];

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}


//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
/*
pagestart = first page since last os page
freemapmax = max index for freemap array
nfreepages = how many free pages available in the system not including os pages
set every entry in freemap to 0
for all free pages:
MemorySetFreemap()
*/
  int j; // current page index 
  int i;
  int freepagemax = MEM_MAX_PAGES;
  int pages = NUM_PAGES;
  nfreepages = 0;
  

  //pagestart = first page since last os page
  pagestart = (lastosaddress + MEM_PAGESIZE - 4) / MEM_PAGESIZE;


  //set every entry in freemap to 0
  for(i = 0; i < pages; i++){
    freemap[i] = 0;
  }
  
  //*****Q3 Yuan
  //Set refcounter to 1 for os pages and 0 to any other pages
  for (j = 0; j < pagestart; j++){
    page_refcounters[j] = 1;
  }

  //for all free pages, increment nfreepages
  for(j = pagestart; j < freepagemax; j ++){
    //set the freemap here, the bit is 1 (the beginning)
    MemorySetFreemap(j, 1);

    //increment the counter
    nfreepages += 1;
  }


}
//set freemap 
inline void MemorySetFreemap(int page, int bit){
  uint32 page_num = page / 32;
  uint32 bitpos = page % 32; //offset

  freemap[page_num] = freemap[page_num] & invert(1 << bitpos);
  freemap[page_num] = freemap[page_num] | (bit  << bitpos);
  return;
}





//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  /*
  find pagenum from addr
find offset value from addr
check given address is less than the max vaddress
lookup PTE in pcb’s page table
if PTE is not valid
save addr to the currentSavedFrame
page fault handler
get and return physical address
  */
  int pagenum = addr / MEM_PAGESIZE;
  int offset = addr % MEM_PAGESIZE;
  uint32 physical_addr = (pcb->pagetable[pagenum] & MEM_PTE_MASK) + offset;
  if(addr < MEM_MAX_VIRTUAL_ADDRESS){
    //lookup PTE in pcb's page table
    if(!(pcb->pagetable[pagenum] & MEM_PTE_VALID)){
      /*save addr to the currentSavedFrame
page fault handler
get and return physical address*/
      pcb->currentSavedFrame[PROCESS_STACK_FAULT] = addr;
      //get the memory page handler
      if(MemoryPageFaultHandler(pcb) == MEM_FAIL){
        printf("MemoryTranslateUsertoSystem function error: Handle page fault failed\n");
        return 0;
      }

    }
  }
  else{
    //the address is full, can exceed
    printf("Memory Translate User to System function error: the address exceeds max\n");
    ProcessKill();
    return 0;
  }

  return physical_addr;
}


//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page 
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);
    
    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault 
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page 
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that 
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference. 
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  /*
  grab faulting addr from the currentSavedFrame
grab user stack addr from the currentSavedFrame
find pagenum for the faulting addr
find pagenum for the user stack
segfault (kill the process) if the faulting address is not part of the stack
else allocate a new physical page, setup its PTE, and insert to pagetable
  */

  uint32 fault;
  uint32 ustack;
  int f_pagen;
  int us_pagen;


  fault = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  ustack = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER];

  f_pagen = fault / MEM_PAGESIZE;
  us_pagen = ustack / MEM_PAGESIZE;

  if(f_pagen < us_pagen){
    printf("segfault (kill the process) in function MemoryPageFaultHandler\n");
    ProcessKill();
    return MEM_FAIL;
  }
  //alocate a new physical page setup its PTE
  else{
    pcb->pagetable[f_pagen] = MemorySetupPte(MemoryAllocPage());
    //increment the page count
    pcb->npages += 1;
    return MEM_SUCCESS;
  }


}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------

int MemoryAllocPage(void) {
/*
return 0 if no free pages
find the available bit in freemap
set it to unavailable
decrement number of free pages
return this allocated page number
*/
  int page = 0;
  uint32 bitpos = 0;
  uint32 page_num;
  //return - if no free pages
  if(nfreepages == 0){
    printf("number of freepages are 0 in the memoryallocpage function\n");
    return 0;
  }

  //find the available slot in freemap
  while(freemap[page] == 0){
    page += 1;
  }

  //set to unava
  page_num = freemap[page];
  while((page_num & (1 << bitpos)) == 0){
    bitpos += 1;
  }
  //set to 0 means unaval

  page_num = (page * 32) + bitpos;
  MemorySetFreemap(page_num, 0);
  //decrement number of free pages
  nfreepages -= 1;

  //*****Q3 Yuan 
  // after finding the available bit in freemap, set the reference count for this page to 1
  page_refcounters[page_num] = 1;
  //return the page num
  return page_num;
}
void MemoryFreePte(uint32 pte){
  //*****Q3 Yuan
  if (page_refcounters[((pte & MEM_PTE_MASK) / MEM_PAGESIZE)] < 1){
    ProcessKill();
  }
  else{
    page_refcounters[((pte & MEM_PTE_MASK) / MEM_PAGESIZE)] -= 1;
  }

  if (page_refcounters[((pte & MEM_PTE_MASK) / MEM_PAGESIZE)] == 0){
    MemoryFreePage((pte & MEM_PTE_MASK) / MEM_PAGESIZE);
  }
}

uint32 MemorySetupPte (uint32 page) {
  //given physical page number, return pte with correct valid status bit
  uint32 pte;

  pte = (page * MEM_PAGESIZE) | MEM_PTE_VALID;
  return pte;
}


void MemoryFreePage(uint32 page) {
  /*
  MemorySetFreemap for given page
increment number of free pages*/

  MemorySetFreemap(page, 1);

  //increment
  nfreepages += 1;
  return;
}

void *malloc(){
  return NULL;
}

int mfree(){
  return 0;
}

//*****Q3 Yuan
void refcounter_inc(uint32 addr){
  uint32 page;

  page = addr / MEM_PAGESIZE;
  page_refcounters[page] += 1;
}

//-----------------------------------------------------------------------------
// This function is called whenever a process tries to access a page that 
// is marked as “readonly” in its page table. If no one else refers to this page (refcounter == 1), 
// then we can just mark it as READONLY in the page table. Otherwise, we have to allocate a new page, 
// copy the entire old page to the new page, decrement the reference coutner for the old page, 
// mark the new page as READONLY, and put the new pte into the page table to replace the old pte 
//-----------------------------------------------------------------------------
int MemoryROPAccessHandler(PCB *pcb) {
  uint32 fault_addr; // fault address from pcb
  uint32 l1_page;    // l1_page_num
  uint32 pte;        //pte associated with this l1_page_num
  uint32 phy_page;   // physical_page_num
  uint32 new_page;
  int pid;
  int i;
  //find fault address from pcb
  fault_addr = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  
  //find l1_page_num for this fault_addr
  l1_page = fault_addr / MEM_PAGESIZE;
  
  //find pte associated with this l1_page_num
  pte = pcb->pagetable[l1_page] & MEM_PTE_MASK;
  
  //find physical_page_num
  phy_page = pte / MEM_PAGESIZE;
  
  if (page_refcounters[phy_page] < 1){
    ProcessKill();
    return MEM_FAIL;
  }

  //nobody else is referring to this page
  if (page_refcounters[phy_page] == 1){
    pcb->pagetable[l1_page] &= ~MEM_PTE_READONLY;
  }
  //other process are referring to this page
  else{
    //allocate a new page
    new_page = MemoryAllocPage();
    
    //copy old data to the new page via “bcopy”
    bcopy((void*)pte, (void*)(new_page * MEM_PAGESIZE), MEM_PAGESIZE);

    //setup pte for this new page
    pcb->pagetable[l1_page] = MemorySetupPte(new_page);

    //decrement refcounter
    page_refcounters[phy_page] -= 1;
  }

  //*****Q4 Yuan:
  pid = GetPidFromAddress(pcb); 
  printf("Process(%d): sysStackPtr: %x\n", pid, pcb->sysStackPtr);
  printf("Process(%d): sysStackArea: %x\n", pid, pcb->sysStackArea);
  printf("Process(%d): current saved frame: %x\n", pid, pcb->currentSavedFrame);
  for (i = 0; i < MEM_L1TABLE_SIZE; i++){
    if (pcb->pagetable[i]) {
      printf("Process(%d): %d's pte: %d\n", pid, i, pcb->pagetable[i]);
    }
  }
  return MEM_SUCCESS;
}