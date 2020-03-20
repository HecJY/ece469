#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done


  //init the sem 
  
  mbox_t s_mbox;
  mbox_t o2_mbox;
  mbox_t so4_mbox;
  int num;

  char* msg;

  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" h2so4 form failed\n"); 
    Exit();
  } 




  //convert the command line output
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  s_mbox = dstrtol(argv[2], NULL, 10);
  o2_mbox = dstrtol(argv[3], NULL, 10);
  so4_mbox = dstrtol(argv[4], NULL, 10);
  num = dstrtol(argv[5], NULL, 10);

  /*Reaction here S + 2 O2 -> SO4  */ 
  mbox_recv(s_mbox, NULL, msg);
  mbox_recv(o2_mbox, NULL, msg);
  mbox_recv(o2_mbox, NULL, msg);

  //produce the so4
  mbox_send(so4_mbox, NULL, "SO4");



  
  Printf("(%d) s + 2*O2 -> SO4 reacted, PID: %d\n", num, getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
