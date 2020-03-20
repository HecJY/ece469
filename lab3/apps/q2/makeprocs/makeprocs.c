
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
  mbox_t s2_mbox;
  mbox_t s_mbox;
  mbox_t o2_mbox;
  mbox_t c2_mbox;
  mbox_t co_mbox;
  mbox_t so4_mbox;
  
  int i;

  
  //correpsonding str
  char s2_mbox_str[10];
  char s_mbox_str[10];
  char o2_mbox_str[10];
  char c2_mbox_str[10];
  char co_mbox_str[10];
  char so4_mbox_str[10];


  //molecule num
  int s2;
  int co;
  int s;
  int o2;
  int c2;
  int so4;

  int s_left;
  int o2_left;
  int c2_left;
  int s2_left;
  int co_left;


  char o2_left_str[10];
  char c2_left_str[10];
  char s2_left_str[10];
  char co_left_str[10];
  char s_left_str[10];
  char num_str[10];

  if (argc != 3) {
    Printf("Usage: "); Printf(argv[0]); Printf(" <number of processes to create> \n");
    Exit();
  }
  //get number of H2O modules, SO4
  s2 = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  co = dstrtol(argv[2], NULL, 10); // the "10" means base 10

  // Convert string from ascii command line argument to integer number
  
  Printf("Creating %d S2s and %d COs.\n", s2, co);

  // Initiallize the value of semaphore
  if ((s_procs_completed = sem_create(-(numprocs-1))) == SYNC_FAIL) {
    Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    Exit();
  }

  //calculate how many so4 will be formed
  s = 2 * s2;
  o2 = floor(co / 4) * 2;
  c2 = floor(co / 4) * 2; 
  
  

  so4 = min(floor(o2 / 2), s);

  o2_left = o2 - 2 * so4; 
  s_left = s - so4;
  s2_left = floor(s2_left / 2);
  c2_left = floor(co / 2);
  co_left = co - floor(co / 4);



  //create the mailboxes
  if((co_mbox = mbox_create()) == MBOX_FAIL){
    Printf("Create co mbox failed");
    Exit();
  }

    //create the mbox
  if((s_mbox = mbox_create()) == MBOX_FAIL){
    Printf("Create S  mbox failed");
    Exit();
  }
  //create the mbox
  if((o2_mbox = mbox_create()) == MBOX_FAIL){
    Printf("Create o2 mbox failed");
    Exit();
  }
  //create the mbox
  if((so4_mbox = mbox_create()) == MBOX_FAIL){
    Printf("Create so4 mbox failed");
    Exit();
  }
  //create the mbox
  if((s2_mbox = mbox_create()) == MBOX_FAIL){
    Printf("Create s2 mbox failed");
    Exit();
  }
  //create the mbox
  if((c2_mbox = mbox_create()) == MBOX_FAIL){
    Printf("Create co mbox failed");
    Exit();
  }

  //Open the mailboxes created
  if(mbox_open(co_mbox) == MBOX_FAIL){
    Printf("open s2 mbox failed");
    Exit();
  }

  if(mbox_open(s2_mbox) == MBOX_FAIL){
    Printf("open h2 mbox failed");
    Exit();
  }

  if(mbox_open(o2_mbox) == MBOX_FAIL){
    Printf("open o2 mbox failed");
    Exit();
  }

  if(mbox_open(so4_mbox) == MBOX_FAIL){
    Printf("open so2 mbox failed");
    Exit();
  }

  if(mbox_open(s_mbox) == MBOX_FAIL){
    Printf("open h2so4 mbox failed");
    Exit();
  }
  if(mbox_open(c2_mbox) == MBOX_FAIL){
    Printf("open co mbox failed");
    Exit();
  }



  //transfer to chars
  ditoa(s_procs_completed,s_procs_completed_str);
  ditoa(s2_mbox, s2_mbox_str);
  ditoa(co_mbox, co_mbox_str);
  ditoa(s_mbox, s_mbox_str);
  ditoa(so4_mbox, so4_mbox_str);
  ditoa(c2_mbox, c2_mbox_str);
  ditoa(o2_mbox, o2_mbox_str);





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


  //create the five processes, including the injection s2, injection co, 
  //process_create();
  //create s2 injection process
  /*
  process_create(H2OINJECTION, s_procs_completed_str, h2o_sem_str, s2_str, NULL);

  //create co injection process
  process_create(SO4INJECTION, s_procs_completed_str, so4_sem_str, so4_str, NULL);

  //reaction one process
  process_create(H2OBREAK, s_procs_completed_str, h2o_sem_str, h2_sem_str, o2_sem_str, s2_str, NULL);

  //r2
  process_create(SO4BREAK, s_procs_completed_str, so4_sem_str, so2_sem_str, o2_sem_str, so4_str, NULL);

  //r3
  process_create(H2SO4FORM, s_procs_completed_str, so2_sem_str, h2_sem_str, o2_sem_str, h2so4_str, h2so4_sem_str,NULL);
*/
  //you should not have any loops inside the generator and reaction programs. Instead, your makeprocs process will create the proper number of each type of process.
  //s2 injection
  for(i = 0; i < s2; i ++){
    process_create(S2INJECTION, s_procs_completed_str, s2_mbox_str, NULL);
  }

  //co injection
  for(i = 0; i < co; i ++){
    process_create(COINJECTION, s_procs_completed_str, co_mbox_str, NULL);
  }

  //first reaction S2 -> S + S
  for(i = 0; i < s2; i ++){
    process_create(S2BREAK, s_procs_completed_str, s2_mbox_str, s_mbox_str, NULL);
  }

  //second co break
  for(i = 0; i < floor(co / 4); i ++){
    process_create(COBREAK, s_procs_completed_str, co_mbox_str, o2_mbox_str, c2_mbox_str, NULL);
  }

  //third forming of so4
  for(i = 0; i < so4; i ++){
    ditoa(i, num_str);
    process_create(SO4FORM, s_procs_completed_str, s_mbox_str, o2_mbox_str, so4_mbox_str, num_str, NULL);
  }

  //close the mailboxes created
  if(mbox_close(co_mbox) == MBOX_FAIL){
    Printf("close s2 mbox failed");
    Exit();
  }

  if(mbox_close(s2_mbox) == MBOX_FAIL){
    Printf("close h2 mbox failed");
    Exit();
  }

  if(mbox_close(o2_mbox) == MBOX_FAIL){
    Printf("close o2 mbox failed");
    Exit();
  }

  if(mbox_close(so4_mbox) == MBOX_FAIL){
    Printf("close so2 mbox failed");
    Exit();
  }

  if(mbox_close(s_mbox) == MBOX_FAIL){
    Printf("close h2so4 mbox failed");
    Exit();
  }
  if(mbox_close(c2_mbox) == MBOX_FAIL){
    Printf("close co mbox failed");
    Exit();
  }


  // And finally, wait until all spawned processes have finished.
  if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
    Exit();
  }

  Printf("%d S2's left over. %d CO's left over. %d S's left over. %d O2's left over.", s2_left, co_left, s_left, o2_left);
  Printf("%d H2SO4's created.\n",  so4);
}