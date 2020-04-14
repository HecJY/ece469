#ifndef	_memory_h_
#define	_memory_h_

// Put all your #define's in memory_constants.h
#include "memory_constants.h"

extern int lastosaddress; // Defined in an assembly file

//--------------------------------------------------------
// Existing function prototypes:
//--------------------------------------------------------

int MemoryGetSize();
void MemoryModuleInit();
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr);
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir);
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from, unsigned char *to, int n);
int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from, unsigned char *to, int n);
int MemoryPageFaultHandler(PCB *pcb);

//---------------------------------------------------------
// Put your function prototypes here

int MemoryAllocPage();  //return 0 if no free pages
inline void MemorySetFreemap(int page, int bit); //given the page number, set the corresponding bit in freemap[]
void MemoryFreePage(uint32 gpage); //memorysetfreemap for given page, increment number of free pages
void MemoryFreePte(uint32 pte); // memory freepage for given pte
uint32 MemorySetupPte(uint32 gpage);
void *malloc();
int mfree();




//---------------------------------------------------------
// All function prototypes including the malloc and mfree functions go here

#endif	// _memory_h_
