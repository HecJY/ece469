#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  //init the semaphores
  sem_t so4_sem;
  sem_t so2_sem;
  sem_t o2_sem;

  //init number of h2os
  int so4_num;

  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  so4_sem = dstrtol(argv[2], NULL, 10);
  so2_sem = dstrtol(argv[3], NULL, 10);
  o2_sem = dstrtol(argv[4], NULL, 10);
  so4_num = dstrtol(argv[5], NULL, 10);

  //Printf("so4 %d\n", so4_num);
  //decrement each time so4 runs throuh
  while(so4_num >= 1){
    //one so4 molecules inside
      if(sem_wait(so4_sem) == SYNC_SUCCESS){
        //generate the so2 molecules and o2 molecules(reaction 2)
        sem_signal(so2_sem);
        sem_signal(o2_sem);

        //if so4 is not decremented, does not change the so4_num, just wait on the so4 sem generated
        so4_num -= 1;

        Printf("SO4 -> SO2 + O2 reacted, PID: %d\n", getpid());
      }
  }


  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
