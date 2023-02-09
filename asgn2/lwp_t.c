#include "lwp.h"
#include "rr_scheduler.h"
#include <stdio.h>

static void test(void*arg){
    int i = 5;
    printf("in test %p\n", arg);
    lwp_exit(i);
}

int main(int argc, char * argv[]){
    long i = 1;
    lwp_create((lwpfun)test,(void*)i);
    lwp_start();
    return 0;
}