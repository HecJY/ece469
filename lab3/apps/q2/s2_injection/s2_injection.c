#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  //init the mailbox
  mbox_t s2_mbox;
  unsigned int h_mem;
  molecules *mol;

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" The s2 injection process failed \n");
    Exit();
  }

  //get the input
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  h_mem = dstrtol(argv[2], NULL, 10);

  //map
  mol = (molecules *) shmat(h_mem);
  if(mol == NULL){
    Printf("Map failed in SO injection \n");
    Exit();
  }

  //open mail box

  if(mbox_open(mol->s2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition\n");
    Exit();
  }

  //send the msg, injection
  /*
  if(mbox_send(mol->s2_mbox, 2, (void *)"S2") != MBOX_SUCCESS){
    Printf("Injection S2 failed, current PID is %d \n", getpid());
  }
*/
  mbox_send(mol->s2_mbox, 2, (void *)"S2");
  //close the mailbox
  if(mbox_close(mol->s2_mbox) != MBOX_SUCCESS){
    Printf("Failed to close s2 mailbox\n");
    Exit();
  }


  Printf("S2 injected into Radeon atmosphere, PID: %d\n", getpid());
  
  

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
