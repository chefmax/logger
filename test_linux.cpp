#include "logger.h"
#include <string>
#define NUM 10

void *theThread (void *param) {
  int d = * (( int* ) param);
  int a[10];
  a[0] = 23;
  if (a[0] > 0 ){
    ++a[0];
  }
  log_error("%d %s",d,"fdgdfgzxczxczxczcdfgg");
  log_info("%d",d);
  int j = 0;
  for (int i = 0; i < 9000000; ++i) {
    j = 55;
    if (j == -1){
      break;
    }
  }
  raise(SIGPIPE);
  raise(SIGSEGV);
  return NULL;
}

int main()
{
  int i = 0;
  pthread_t  thread[NUM];
  int d = 5555555; 
  int res = 0;
  for (i=0; i<NUM; i++) {
     res = pthread_create(&thread[i], NULL, theThread, (void*) &i);
  }

  log_error("%d %s",d,"edrfqdtyudyudyudyudyudyudyuwertqwt4wertqwertqwerttt");
  log_info("%d",d);
  raise(SIGILL);
  //array[3456] = 4;
  for (i=0; i <NUM; i++) {
    res = pthread_join(thread[i], NULL); 
  }
  if (res<0) {
    return 1;
 	
  }
  close_log_file();
  return 0; 
}

