#ifndef __USERPROG__
#define __USERPROG__

#define LEN 11
typedef struct circular_buffer{
  int len = LEN; //because hello worlds' str length
  char buffer[LEN];
  int numprocs;
  int head;
  int tail;
} circular_buffer;

#define FILENAME_TO_RUN "spawn_me.dlx.obj"

#endif
