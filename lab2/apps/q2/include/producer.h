#ifndef __USERPROG__
#define __USERPROG__

#define LEN 11
typedef struct circular_buffer{
  char buffer[LEN];
  int numprocs;
  int head;
  int tail;
} circular_buffer;

#define PRODUCER "producer.dlx.obj"
#define CONSUMER "consumer.dlx.obj"

#endif
