#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>


#include "lwp.h"

int id_count = 1;

tid_t lwp_create(lwpfun function,void * argument){
    context * new_thread;
    unsigned long *s;
    long r, page_size;
    //default stack size 8 MB
    int howbig = 8000000;
    page_size = getpagesize();
    struct rlimit limit_struct;
    r = getrlimit(RLIMIT_STACK, &limit_struct);
    //setting stack to stack resource limit 
    if(limit_struct.rlim_cur && limit_struct.rlim_cur != RLIM_INFINITY){
        if(limit_struct.rlim_cur % page_size == 0){
            howbig = limit_struct.rlim_cur;
        }
        else{
            //ensuring it is a multiple of page size 
            howbig = limit_struct.rlim_cur + page_size + (limit_struct.rlim_cur % page_size);
        }
        
    }
    new_thread->stacksize = howbig;

    //create stack 
    s = mmap(NULL,howbig,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    if(s == MAP_FAILED){
        printf("unable to map region\n");
        return NO_THREAD;
    }
    new_thread->stack=(s + (howbig/sizeof(unsigned long)));
    // if(s % 16 != 0){
    //     printf("stack is not alligned on 16 byte boundary\n");
    // }
    new_thread->tid = id_count++;

    /*  
        initialize stack and registers as if 
        function called swap_rfiles() itself 
    */
    *(new_thread->stack) = (unsigned long)function; /* put function on stack to mimic return address */

    new_thread->state.rbp = (unsigned long)(new_thread->stack);           /* base of stack address */
    new_thread->state.rsp = (unsigned long)(new_thread->stack - 1);     /* top of stack address */
    new_thread->state.rdi = (unsigned long)argument;                      /* argument -- should this be passed as address? */

    new_thread->state.fxsave=FPU_INIT; /*floating point state */
}