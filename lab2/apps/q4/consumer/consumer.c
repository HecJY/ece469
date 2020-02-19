#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  circular_buffer *cb;        // Used to access missile codes in shared memory page
  uint32 h_mem;            // Handle to the shared memory page
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done
  int i;
  //int cur;
  cond_t cond_empty;
	cond_t cond_full;


  lock_t lock;
  if (argc != 6) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  lock = dstrtol(argv[3], NULL, 10);
  cond_full = dstrtol(argv[4], NULL, 10);
	cond_empty = dstrtol(argv[5], NULL, 10);


  // Map shared memory page into this process's memory space
  if ((cb = (circular_buffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }



  for(i = 0; i < LEN; i ++){
		//acquire lock

		while(lock_acquire(lock) != SYNC_SUCCESS){
			Printf("Lock acquired failed\n");
		}
		//check emptywhile (pool is empty


    while (cb->tail == cb->head) {
      Printf("inside of the tail and head code");
			cond_wait(cond_empty);
		}
		//remove(item from pool)
		//consume the item

		Printf("Consumer %d removed: %c\n\n", getpid(), cb->buffer[cb->tail]);
    cb->tail = (cb->tail + 1) % LEN;
    

    cond_signal(cond_full);
		//release (lock)
		while(lock_release(lock) != SYNC_SUCCESS){
			Printf("Lock release failed\n");
		}
	
	}
  // Now print a message to show that everything worked
  //Printf("spawn_me: This is one of the %d instances you created.  ", cb->numprocs);
  //Printf("spawn_me: My PID is %d\n", getpid());

  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
