#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  //uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  //init the semaphores
  mbox_t s2_mbox;
  mbox_t s_mbox;

  //init the message in the mbox
  char message[2];


  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 
  s_procs_completed = dstrtol(argv[1], NULL, 10);
  s2_mbox = dstrtol(argv[2], NULL, 10);
  s_mbox = dstrtol(argv[3], NULL, 10);

  //open the mailbox
  if(mbox_open(s2_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_open(s_mbox) != MBOX_SUCCESS){
    Printf("Open the mailbox failed, check the condition");
    Exit();
  }

  //reaction here, consume
  mbox_recv(s2_mbox, 2,  message);

  //produce the twos 
  mbox_send(s_mbox, 1, "S");
  mbox_send(s_mbox, 1, "S");



  //close the mailbox after it finished
  if(mbox_close(s2_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }

  if(mbox_close(s_mbox) != MBOX_SUCCESS){
    Printf("Close the mailbox failed, check the condition");
    Exit();
  }


  Printf("S2 -> S + S reacted, PID: %d\n", getpid());

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
