#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int so4_num;

  //init the sems
  sem_t so4_sem;

  if (argc != 4) {
    Printf("Usage: "); Printf(argv[0]); Printf(" The h2o injection process failed \n");
    Exit();
  }

  //get the input
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  so4_sem = dstrtol(argv[2], NULL, 10);
  so4_num = dstrtol(argv[3], NULL, 10);

  while(so4_num > 0){
    //increment the h2o sem
    sem_signal(so4_sem);

    so4_num --;

    Printf("SO4 injected into Radeon atmosphere, PID: %d\n", getpid());
  }
  

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
