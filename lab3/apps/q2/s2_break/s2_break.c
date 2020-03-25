#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  //init the semaphores

  unsigned int h_mem;
  molecules *mol;

  //init the message in the mbox
  char message[2];


  if (argc != 3) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  h_mem = dstrtol(argv[2], NULL, 10);

    //map
  mol = (molecules *) shmat(h_mem);
  if(mol == NULL){
    Printf("Map failed in SO injection \n");
    Exit();
  }

  //open the mailbox
  if(mbox_open(mol->s2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }
   //reaction here, consume
  mbox_recv(mol->s2_mbox, 2,  (void *)message);


  //close the mailbox after it finished
  if(mbox_close(mol->s2_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_open(mol->s_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

 


  //produce the twos 
  mbox_send(mol->s_mbox, 1, (void *) "S");
  mbox_send(mol->s_mbox, 1, (void *) "S");




  if(mbox_close(mol->s_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }


  Printf("S2 -> S + S reacted, PID: %d\n", getpid());

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
