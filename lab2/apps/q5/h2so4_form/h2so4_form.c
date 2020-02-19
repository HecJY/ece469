#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done


  //init the sem 
  
  sem_t so2_sem;
  sem_t h2_sem;
  sem_t o2_sem;
  int h2so4_num;
  sem_t h2so4_sem;
  int num = 1;



  if (argc != 7) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" h2so4 form failed\n"); 
    Exit();
  } 




  //convert the command line output
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  so2_sem = dstrtol(argv[2], NULL, 10);
  h2_sem = dstrtol(argv[3], NULL, 10);
  o2_sem = dstrtol(argv[4], NULL, 10);
  h2so4_num = dstrtol(argv[5], NULL, 10);
  h2so4_sem = dstrtol(argv[6], NULL, 10);
  

  while(h2so4_num > 0){
    
    if(sem_wait(so2_sem) != SYNC_SUCCESS){
      Printf("bad sem created\n");
      Exit();
    }

    if(sem_wait(h2_sem) != SYNC_SUCCESS){
      Printf("bad sem created\n");
      Exit();
    }

    if(sem_wait(o2_sem) != SYNC_SUCCESS){
      Printf("bad sem created\n");
      Exit();
    }
    //Printf("1sdhsadhsad\n");
    if(sem_signal(h2so4_sem) == SYNC_SUCCESS){
      h2so4_num --;
      Printf("(%d) H2 + O2 + SO2 -> H2SO4 reacted, PID: %d\n", num, getpid());
      num += 1;
    }


  }

  

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
