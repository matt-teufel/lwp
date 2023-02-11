/*
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lwp.h"

static void indentnum(uintptr_t num) {
  /* print the number num num times, indented by 5*num spaces
   * Not terribly interesting, but it is instructive.
   */
  printf("Greetings from Thread %d.  Yielding...\n",(int)num);
  lwp_yield();
  printf("I'm still alive.  Goodbye\n");
  return;
}


int main(int argc, char *argv[]){
  long i;

  /* spawn a number of individual LWPs */
  for(i=0;i<1;i++)
    lwp_create((lwpfun)indentnum,(void*)i);

  lwp_start();
  lwp_wait(NULL);

  printf("After thread termination\n");
  return 0;
}


