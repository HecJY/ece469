#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

static mbox mboxes[MBOX_NUM_MBOXES];
static mbox_message mbox_messages[MBOX_NUM_BUFFERS];
//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
  int i;
  for (i = 0; i < MBOX_NUM_MBOXES; i++){
    mboxes[i].inuse = 0;
  }
  
  for (i = 0; i < MBOX_NUM_BUFFERS; i++){
    mbox_messages[i].inuse = 0;
  }

}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
  mbox_t mbox;
  uint32 intrval;
  int i;
  // grabbing a mailbox should be an atomic operation
  intrval = DisableIntrs();
  for(mbox=0; mbox<MBOX_NUM_MBOXES; mbox++) {
    if(mboxes[mbox].inuse==0) {
      mboxes[mbox].inuse = 1;
      break;
    }
  }
  RestoreIntrs(intrval);

  // Attach the lock
  if ((mboxes[mbox].lock = LockCreate()) == SYNC_FAIL){
    printf("ERROR: Could not create lock\n");
    exitsim();
  }

  // Create 2 conditional variable
  if ((mboxes[mbox].notfull = CondCreate(mboxes[mbox].lock)) == SYNC_FAIL){
    printf("ERROR: Could not create conditional varaiables--not full \n");
    exitsim();
  } 
  
  if ((mboxes[mbox].notempty = CondCreate(mboxes[mbox].lock)) == SYNC_FAIL){
    printf("ERROR: Could not create conditional varaiables-- not empty \n");
    exitsim();
  } 
    
  // Initialize the message queue
  if (AQueueInit(&mboxes[mbox].message_q) != QUEUE_SUCCESS){
    printf("FATAL ERROR: Could not initialize mbox messsage queue\n");
    exitsim();
  }

  for (i = 0; i <PROCESS_MAX_PROCS; i++){
    mboxes[mbox].proc[i] = 0;
  }

  if (mbox == MBOX_NUM_MBOXES) return MBOX_FAIL;

  return mbox;
}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
  // Get Pid
  int currentPid = GetCurrentPid();
  
  // Use the lock
  if (LockHandleAcquire(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockAcquire\n");
    return MBOX_FAIL;
  }

  if (mboxes[handle].inuse == 0){
    return MBOX_FAIL;
  }

  // Add to proc list
  mboxes[handle].proc[currentPid] = 1; 

  // Release the lock
  if (LockHandleRelease(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockRelease\n");
    return MBOX_FAIL;
  }
  
  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
  // Get Pid
  int currentPid = GetCurrentPid();
  int proc_inuse = 0;
  int i;

  // Use the lock
  if (LockHandleAcquire(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockAcquire\n");
    return MBOX_FAIL;
  }

  if (mboxes[handle].inuse == 0){
    return MBOX_FAIL;
  }

  // Remove from proc list
  mboxes[handle].proc[currentPid] = 0; 

  // Check if otehr process is using the mbox
  for (i = 0; i < PROCESS_MAX_PROCS; i++){
    proc_inuse += (mboxes[handle].proc[i] == 1);
  }

  // Otherwise free the resources
  if (proc_inuse == 0){
    mboxes[handle].inuse = 0;
    AQueueInit(&(mboxes[handle].message_q));
  }

  // Release the lock
  if (LockHandleRelease(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockRelease\n");
    return MBOX_FAIL;
  }
  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
  // Get Pid
  //int currentPid = GetCurrentPid();
  //int proc_inuse = 0;
  int i;
  Link* l;

  // Use the lock
  if (LockHandleAcquire(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockAcquire\n");
    return MBOX_FAIL;
  }

  if(mboxes[handle].inuse == 0){
    return MBOX_FAIL;
  }

  //If mboxes is Full, wait not Full
  if(mboxes[handle].message_q.nitems >= MBOX_MAX_BUFFERS_PER_MBOX){
    CondHandleWait(mboxes[handle].notfull);
  }

  // Get unused buffer
  for(i=0; i<MBOX_NUM_MBOXES; i++) {
    if(mboxes[i].inuse==0) {
      mboxes[i].inuse = 1;
      break;
    }
  }

  // Set length and copy data
  mbox_messages[i].length = length; 
  bcopy(message, mbox_messages[i].message, 8);
  
  if ((l = AQueueAllocLink(mbox_messages[i].message)) == NULL){
    return MBOX_FAIL;
  }

  // Add to the queue
  AQueueInsertFirst(&mboxes[handle].message_q, l);

  // Signal not Empty
  if (CondHandleSignal(mboxes[handle].notempty) != SYNC_SUCCESS) {
    return MBOX_FAIL;
  }

  // Release the lock
  if (LockHandleRelease(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockRelease\n");
    return MBOX_FAIL;
  }
  return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
  // Get Pid
  int currentPid = GetCurrentPid();
  //int proc_inuse = 0;
  //int i;
  Link* l;
  mbox_message* mbox_recv;

  // Use the lock
  if (LockHandleAcquire(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockAcquire\n");
    return MBOX_FAIL;
  }

  if(mboxes[handle].inuse == 0){
    return MBOX_FAIL;
  }

  //check the mbox is open or not
  if(mboxes[handle].proc[currentPid] == 0){
    return MBOX_FAIL;
  }

  if (AQueueEmpty( &mboxes[handle].message_q)) {
    if(CondHandleWait(mboxes[handle].notempty)){
      l = AQueueFirst(&mboxes[handle].message_q);
      mbox_recv = (mbox_message*) l->object;
    }
  }

  if (mbox_recv->length > maxlength){
    return MBOX_FAIL;
  }

  bcopy(mbox_recv->message, message, mbox_recv->length);
  mbox_recv->inuse = 0;
  AQueueRemove(&l);

  // Signal not Full
  if (CondHandleSignal(mboxes[handle].notfull) != SYNC_SUCCESS) {
    return MBOX_FAIL;
  }

  // Release the lock
  if (LockHandleRelease(mboxes[handle].lock) != SYNC_SUCCESS){
    printf("Fail to LockRelease\n");
    return MBOX_FAIL;
  }
  return MBOX_SUCCESS;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
  int proc_count;
  int i;
  int j;
  //Link* l;

  if (pid > MBOX_NUM_MBOXES || pid < 0){
    return MBOX_FAIL;
  }

  for (i = 0; i < MBOX_NUM_MBOXES; i++){
    if (mboxes[i].proc[pid]){

      // Use the lock
      if(LockHandleAcquire(mboxes[i].lock != SYNC_SUCCESS)){
        printf("Fail to LockAcquire\n");
        return MBOX_FAIL;
      }

      mboxes[i].proc[pid] = 0;

      for (j = 0; j < PROCESS_MAX_PROCS; j++){
        if (mboxes[i].proc[j] == 1){
          proc_count++;
        }
      }

      if (proc_count == 0){
        mboxes[i].inuse = 0;
      }


      if(LockHandleRelease(mboxes[i].lock != SYNC_SUCCESS)){
          printf("Fail to LockRelease\n");
          return MBOX_FAIL;
        }
    }
  }
  return MBOX_SUCCESS;
}
