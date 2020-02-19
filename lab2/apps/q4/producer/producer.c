#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{

	//init handlers for circular buffer
	//circular buffer has to be inited in the producer.h
	
	int i;
  	circular_buffer *cb;        // Used to access circular buffer in shared memory page
  	uint32 h_mem;            // Handle to the shared memory page
  	sem_t s_procs_completed; // Semaphore signal the original process that we're done
	//int cur; //current position of the head
	char input[] = "Hello World";
	cond_t cond_empty;
	cond_t cond_full;
	lock_t lock; //init locks in the process

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
  	//until every char in hello world is produced
	for(i = 0; i < LEN; i ++){
		//acquire lock
		while(lock_acquire(lock) != SYNC_SUCCESS){
			Printf("Lock acquired failed\n");
		}

		//check full
		while (cb->head == (cb->tail + 1) % LEN) {
			cond_wait(cond_full);
		}
		Printf("Producer %d inserted: %c\n", getpid(), input[i]);
		//
		cb->buffer[cb->head] = input[i];
		cb->head = (cb->head + 1) % LEN;
		cond_signal(cond_empty);
		
		
		//release (lock)
		while(lock_release(lock) != SYNC_SUCCESS){
			Printf("Lock release failed\n");
		}
	
	}


  // Now print a message to show that everything worked
  Printf("number of producers: This is one of the %d instances you created.  ", cb->numprocs);

  // Signal the semaphore to tell the original process that we're done
  Printf("producer: PID %d is complete.\n", getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}
