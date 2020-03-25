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

  //open the mailboxes /*Reaction  4 CO -> 2 O2 + 2 C2*/
  if(mbox_open(mol->co_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }
  mbox_recv(mol->co_mbox, 2, message);
  mbox_recv(mol->co_mbox, 2, message);
  mbox_recv(mol->co_mbox, 2, message);
  mbox_recv(mol->co_mbox, 2, message);


  if(mbox_close(mol->co_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }


  if(mbox_open(mol->o2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

    //produce 2 o2
  mbox_send(mol->o2_mbox, 2, "O2");
  mbox_send(mol->o2_mbox, 2, "O2");

      //close the mailboxes


  if(mbox_close(mol->o2_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }
  if(mbox_open(mol->c2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

 




  

  //produce 2 c2
  mbox_send(mol->c2_mbox, 2, (void *) "C2") != MBOX_SUCCESS);
  mbox_send(mol->c2_mbox, 2, (void *) "C2") != MBOX_SUCCESS);




  if(mbox_close(mol->c2_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }


  Printf("4 CO -> 2 O2 + 2 C2 reacted, PID: %d\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
