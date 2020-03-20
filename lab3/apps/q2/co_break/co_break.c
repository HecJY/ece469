#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  //init the semaphores
  mbox_t co_mbox;
  mbox_t o2_mbox;
  mbox_t c2_mbox;
  char * message;
  int i ;
  //init number of h2os

  if (argc != 5) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  s_procs_completed = dstrtol(argv[1], NULL, 10);
  co_mbox = dstrtol(argv[2], NULL, 10);
  o2_mbox = dstrtol(argv[3], NULL, 10);
  c2_mbox = dstrtol(argv[4], NULL, 10);

  //open the mailboxes
  if(mbox_open(co_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_open(o2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_open(c2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

  /*Reaction  4 CO -> 2 O2 + 2 C2*/
  mbox_recv(co_mbox, 2, message);
  mbox_recv(co_mbox, 2, message);
  mbox_recv(co_mbox, 2, message);
  mbox_recv(co_mbox, 2, message);



  //produce 2 o2
  mbox_send(o2_mbox, 2, "O2");
  mbox_send(o2_mbox, 2, "O2");
  

  //produce 2 c2
  mbox_send(c2_mbox, 2, "C2");
  mbox_send(c2_mbox, 2, "C2");


    //close the mailboxes
  if(mbox_close(co_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_close(o2_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_close(c2_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }


  Printf("2 H2O -> 2 H2 + O2 reacted, PID: %d\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
