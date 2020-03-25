#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done


  unsigned int h_mem;
  molecules *mol;

  //init the message in the mbox
  char msg[100];
  int num;

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  h_mem = dstrtol(argv[2], NULL, 10);
  num = dstrtol(argv[3], NULL, 10);
    //map
  mol = (molecules *) shmat(h_mem);
  if(mol == NULL){
    Printf("Map failed in SO injection \n");
    Exit();
  }


  /*Reaction here S + 2 O2 -> SO4  */ 
  mbox_open(mol->s_mbox);
  mbox_recv(mol->s_mbox, 1, msg);
  mbox_close(mol->s2_mbox);


  mbox_open(mol->o2_mbox);
  mbox_recv(mol->o2_mbox, 2, msg);
  mbox_recv(mol->o2_mbox, 2, msg);

  mbox_close(mol->o2_mbox);

  mbox_open(mol->so4_mbox);



  //produce the so4
  mbox_send(mol->so4_mbox, 3, "SO4");


  mbox_close(mol->so4_mbox);


  
  Printf("(%d) s + 2*O2 -> SO4 reacted, PID: %d\n", num, getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
