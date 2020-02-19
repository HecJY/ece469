#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "producer.h"

void main (int argc, char *argv[])
{
  int numprocs = 0;               // Used to store number of processes to create                
  //uint32 h_mem;                   // Used to hold handle to shared memory page

  sem_t s_procs_completed;        // Semaphore used to wait until all spawned processes have completed
  //char h_mem_str[10];             // Used as command-line argument to pass mem_handle to new processes
  char s_procs_completed_str[10]; // Used as command-line argument to pass page_mapped handle to new processes

  //init different semaphores to the molecules
  sem_t h2o_sem;
  sem_t h2_sem;
  sem_t o2_sem;
  sem_t so4_sem;
  sem_t so2_sem;
  sem_t h2so4_sem;
  
  //correpsonding str
  char h2o_sem_str[10];
  char h2_sem_str[10];
  char o2_sem_str[10];
  char so4_sem_str[10];
  char so2_sem_str[10];
  char h2so4_sem_str[10];


  //molecule num
  int h2o;
  int so4;
  int h2so4;
  int o2;
  int h2;
  int so2;

  int o2_left;
  int h2_left;
  int so2_left;
  int h2o_left;

  char h2o_str[10];
  char so4_str[10];
  char h2so4_str[10];

  char o2_left_str[10];
  char h2_left_str[10];
  char so2_left_str[10];
  char h2o_left_str[10];


  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of processes to create> \n");
    Exit();
  }
  //get number of H2O modules, SO4
  h2o = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  so4 = dstrtol(argv[2], NULL, 10); // the "10" means base 10

  // Convert string from ascii command line argument to integer number
  
  Printf("Creating %d H2Os and %d SO4s.\n", h2o, so4);

  // Initiallize the value of semaphore
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  //calculate how many h2so4 will be formed
  o2 = so4 + h2o / 2;
  so2 = so4;
  h2 = h2o / 2;
  h2 *= 2;

  h2so4 = min(min(o2,so2), h2);

  o2_left = o2 - h2so4; 
  h2_left = h2 - h2so4;
  so2_left = so2 - h2so4;
  h2o_left = h2o - h2so4;



  //create the semaphores
  if((so4_sem = sem_create(0)) == SYNC_FAIL){
    Printf("Create so4 semaphor failed");
    Exit();
  }

    //create the semaphores
  if((h2_sem = sem_create(0)) == SYNC_FAIL){
    Printf("Create h2 semaphor failed");
    Exit();
  }
  //create the semaphores
  if((o2_sem = sem_create(0)) == SYNC_FAIL){
    Printf("Create o2 semaphor failed");
    Exit();
  }
  //create the semaphores
  if((h2o_sem = sem_create(0)) == SYNC_FAIL){
    Printf("Create h2o semaphor failed");
    Exit();
  }
  //create the semaphores
  if((h2so4_sem = sem_create(0)) == SYNC_FAIL){
    Printf("Create h2so4 semaphor failed");
    Exit();
  }
  //create the semaphores
  if((so2_sem = sem_create(0)) == SYNC_FAIL){
    Printf("Create so2 semaphor failed");
    Exit();
  }



  //transfer to chars
  ditoa(s_procs_completed,s_procs_completed_str);
  ditoa(h2o_sem, h2o_sem_str);
  ditoa(h2_sem, h2_sem_str);
  ditoa(so2_sem, so2_sem_str);
  ditoa(so4_sem, so4_sem_str);
  ditoa(h2so4_sem, h2so4_sem_str);
  ditoa(o2_sem, o2_sem_str);
  ditoa(h2o, h2o_str);
  ditoa(so4, so4_str);
  ditoa(h2so4, h2so4_str);

  //left
  ditoa(h2_left, h2_left_str);
  ditoa(h2o_left, h2o_left_str);
  ditoa(o2_left, o2_left_str);
  ditoa(so2_left, so2_left_str);



  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.  To do this, we will initialize
  // the semaphore to (-1) * (number of signals), where "number of signals"
  // should be equal to the number of processes we're spawning - 1.  Once 
  // each of the processes has signaled, the semaphore should be back to
  // zero and the final sem_wait below will return.
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }


  // Now we can create the processes.  Note that you MUST end your call to
  // process_create with a NULL argument so that the operating system
  // knows how many arguments you are sending.


  //create the five processes, including the injection h2o, injection so4, 
  //process_create();
  //create h2o injection process
  process_create(H2OINJECTION, s_procs_completed_str, h2o_sem_str, h2o_str, NULL);

  //create so4 injection process
  process_create(SO4INJECTION, s_procs_completed_str, so4_sem_str, so4_str, NULL);

  //reaction one process
  process_create(H2OBREAK, s_procs_completed_str, h2o_sem_str, h2_sem_str, o2_sem_str, h2o_str, NULL);

  //r2
  process_create(SO4BREAK, s_procs_completed_str, so4_sem_str, so2_sem_str, o2_sem_str, so4_str, NULL);

  //r3
  process_create(H2SO4FORM, s_procs_completed_str, so2_sem_str, h2_sem_str, o2_sem_str, h2so4_str, h2so4_sem_str,NULL);



  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  Printf("%d H2O's left over. %d H2's left over. %d O2's left over. %d SO2's left over.", h2o_left, h2_left, o2_left, so2_left);
  Printf("%d H2SO4's created.\n",  h2so4);
}