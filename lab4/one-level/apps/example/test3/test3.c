#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int page_size = 4096;
  //init a virutal adddress that is over the size
  int vad = 0xFFFFF + 9 - 11*4096;
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("test3 (%d): Access memory inside the virtual address space, but outside of currently allocated pages\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test3 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Access memory inside the virtual address space, but outside of currently allocated pages %d\n", (*((int *)(vad))));
  Printf("test3 (%d): Done!\n", getpid());
}
