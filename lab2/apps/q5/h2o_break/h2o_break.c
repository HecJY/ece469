#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  //init the semaphores
  sem_t h2o_sem;
  sem_t h2_sem;
  sem_t o2_sem;
  int i ;
  //init number of h2os
  int h2o_num;

  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  h2o_sem = dstrtol(argv[2], NULL, 10);
  h2_sem = dstrtol(argv[3], NULL, 10);
  o2_sem = dstrtol(argv[4], NULL, 10);
  h2o_num = dstrtol(argv[5], NULL, 10);

  //decrement each time h2o runs throuh
  while(h2o_num >= 2){
    //two h2o molecules inside
        for(i = 0; i < 2; i ++){
            if(sem_wait(h2o_sem) != SYNC_SUCCESS){
              Printf("bad sem created\n");
              Exit();
            }
        }
        //generate the h2 molecules and o2 molecules(reaction 1)
        if(sem_signal(h2_sem) != SYNC_SUCCESS){
          Printf("bad sem created\n");
          Exit();
        }
        if(sem_signal(h2_sem) != SYNC_SUCCESS){
          Printf("bad sem created\n");
          Exit();
        }
        if(sem_signal(o2_sem) != SYNC_SUCCESS){
          Printf("bad sem created\n");
          Exit();
        }

        //if h2o is not decremented, does not change the h2o_num, just wait on the h2o sem generated
        h2o_num -= 2;

        Printf("2 H2O -> 2 H2 + O2 reacted, PID: %d\n", getpid());

    
  }


  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
