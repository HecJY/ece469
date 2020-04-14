#include "usertraps.h"
#include "misc.h"

#define TEST "test.dlx.obj"

void main (int argc, char *argv[])
{
  //int num_test = 0;             // Used to store number of processes to create
  //int i;                               // Loop index variable
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes
  
  if (argc != 1) {
    Printf("Usage: ", argv[0]);
    Exit();
  }
  
  // Convert string from ascii command line argument to integer number
  //num_test = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  //Printf("makeprocs (%d): Creating %d hello_world processes\n", getpid(), num_test);
  //Printf("Hello\n");
  
  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(-1)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }
  
  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  
  // Create processes
  process_create(TEST, s_procs_completed_str, NULL);
  
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS){
    Printf("makeprocs (%d): Bad sem test\n", getpid());
    Exit();
  }

  Printf("----------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
