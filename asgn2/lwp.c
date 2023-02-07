#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>


#include "lwp.h"

void print_stack(thread t){
    int i = 1;
    while(t->stack[-i] != NULL){
        printf("stack index: %i, value: %lu\n",i, t->stack[-i]);
        i++;
    }
}

void print_registers(thread t){
  printf("rax: %lu\n", t->state.rax);
  printf("rbx: %lu\n", t->state.rbx);
  printf("rcx: %lu\n", t->state.rcx);
  printf("rdx: %lu\n", t->state.rdx);
  printf("rsi: %lu\n", t->state.rsi);
  printf("rdi: %lu\n", t->state.rdi);
  printf("rbp: %lu\n", t->state.rbp);
  printf("rsp: %lu\n", t->state.rsp);
  printf("r8: %lu\n", t->state.r8);
  printf("r9: %lu\n", t->state.r9);
  printf("r10: %lu\n", t->state.r10);
  printf("r11: %lu\n", t->state.r11);
  printf("r12: %lu\n", t->state.r12);
  printf("r13: %lu\n", t->state.r13);
  printf("r14: %lu\n", t->state.r14);
  printf("r15: %lu\n", t->state.r15);
  printf("fxsave: %lu\n", t->state.fxsave);
}

struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};
thread current_lwp = NULL;
void lwp_start(void){
    /*
    Starts the threading system by converting the calling thread—the original system thread—into a
    LWP by allocating a context for it and admitting it to the scheduler, and yields control to whichever
    thread the scheduler indicates. It is not necessary to allocate a stack for this thread since it already
    has one
    */
    thread_context = malloc(sizeof(rfile)); /*allocating context*/
    thread scheduled_thread = rr_publish -> next(); /*creates a thread to be scheduled*/
    swap_rfiles(thread_context, &(scheduled_thread-> state)); /*admit thread_context to scheduler, as thread to be scheduled*/
}
void lwp_exit(int status){
    /*
    Terminates the current LWP and yields to whichever thread the
    scheduler chooses. lwp exit() does not return.
    */
}
void lwp_yield(void){
    /*
    Yields control to another LWP.
    Which one depends on the scheduler.
    Saves the current LWPs context, picks the next one, restores that thread’s context, and returns.
    If there is no next thread, terminates the program
    */
   thread other_thread = current_lwp;
   current_lwp = rr_publish->next();
   swap_rfiles(&(other_thread -> state), &(current_lwp -> state));
}


int id_count = 1;

tid_t lwp_create(lwpfun function,void * argument){
    thread new_thread;
    unsigned long *s;
    long r, page_size;
    //default stack size 8 MB
    int howbig = 8000000;
    page_size = getpagesize();
    struct rlimit limit_struct;
    r = getrlimit(RLIMIT_STACK, &limit_struct);
    //setting stack to stack resource limit 
    if(limit_struct.rlim_cur > 0 && limit_struct.rlim_cur != RLIM_INFINITY){
        if(limit_struct.rlim_cur % page_size == 0){
            howbig = limit_struct.rlim_cur;
        }
        else{
            //ensuring it is a multiple of page size 
            howbig = limit_struct.rlim_cur + page_size + (limit_struct.rlim_cur % page_size);
        }
        
    }
    //create stack 
    s = mmap(NULL,howbig,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    new_thread = mmap(NULL, sizeof(context),PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    if(s == MAP_FAILED){
        printf("unable to map region\n");
        return NO_THREAD;
    }
    new_thread->stacksize = howbig;
    new_thread->stack=(s + (howbig/sizeof(unsigned long)));
    if((unsigned long)s % 16 != 0){
        printf("stack is not alligned on 16 byte boundary\n");
    }
    new_thread->tid = id_count++;

    /*  
        initialize stack and registers as if 
        function called swap_rfiles() itself 
    */
    *(s-1) = (unsigned long)function; /* put function address on stack to mimic return address */
    *(s-2) = (unsigned long)s;          /* put rbp on stack */
    new_thread->state.rsp = (unsigned long)(s - 2);     /* top of stack address */
    new_thread->state.rdi = (unsigned long)argument;/* argument */

    new_thread->state.fxsave=FPU_INIT; /*floating point state */
    

    rr_publish->rr_admit(new_thread);
}

int main(int argc, char * argv[]){
    return 0;
}