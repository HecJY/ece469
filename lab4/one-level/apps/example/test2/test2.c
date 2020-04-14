#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  //init a virutal adddress that is over the size
  int vad = 0xFFFFF + 9;
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Now print a message to show that everything worked
  Printf("test2 (%d): Test the virtual address that is beyond max vaddress\n", getpid());

  // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("test2 (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  Printf("Print the virtual address that is beyond the max virtual address: %d\n", (*((int *)(vad))));
  Printf("test2 (%d): Done!\n", getpid());
}
