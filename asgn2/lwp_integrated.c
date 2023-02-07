#include "lwp.h"
#include "rr_scheduler.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};
scheduler rr = &rr_publish;
thread current_lwp = NULL;
rfile *thread_context;
int id_count = 1;

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


/*TO DO*/

thread tid2thread(tid_t tid){
    /*
    returns the thread corresponding to the given thread ID, or NULL if
    ID is invalid
    */
}

tid_t lwp_wait(int *status){
    /*
    Waits for a thread to terminate, deallocates its resources, and reports its termination status if status is non-NULL.
    Returns the tid of the terminated thread or NO THREAD   
    */
}

void lwp_exit(int exitval){
    /*
    Terminates the current LWP and yields to whichever thread the
    scheduler chooses. lwp exit() does not return    
    */
}

/*
finished functions
*/

tid_t lwp_create(lwpfun function,void * argument){
    /*Creates a new lightweight process which executes the given function
    with the given argument.
    lwp create() returns the (lightweight) thread id of the new thread
    or NO THREAD if the thread cannot be created. */

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


void lwp_start(void){
/*
Starts the threading system by converting the calling thread—the original system thread—into a
LWP by allocating a context for it and admitting it to the scheduler, and yields control to whichever
thread the scheduler indicates. It is not necessary to allocate a stack for this thread since it already
has one
*/

thread_context = malloc(sizeof(rfile)); /*allocating context*/
thread scheduled_thread = rr -> next(); /*creates a thread to be scheduled*/

swap_rfiles(thread_context, &(scheduled_thread-> state)); /*admit thread_context to scheduler, as thread to be scheduled*/
}


void lwp_yield(void){
    /*
    Yields control to another LWP. 
    Which one depends on the scheduler. 
    Saves the current LWPs context, picks the next one, restores that thread’s context, and returns. 
    If there is no next thread, terminates the program
    */
   thread other_thread = current_lwp;
   current_lwp = rr->next();
   
   swap_rfiles(&(other_thread -> state), &(current_lwp -> state));
}

tid_t lwp_gettitd(void){
    /*
    Returns the tid of the calling LWP or NO THREAD if not called by a
    LWP.   
    */
    if (current_lwp == NULL){
        return NO_THREAD;
    }
    else{
        return current_lwp -> tid;
    }
}

void copy(thread old, thread new){
    /*copies old thread data into the new thread
    review this*/
    new->stack = old-> stack;
    new -> stacksize = old-> stacksize;
    new ->tid = old-> tid;

    new->state = old->state;
    new->lib_one = old->lib_one;
    new->lib_two = old->lib_two;
    new->sched_one = old->sched_one;
    new->sched_two = old->sched_two;
}

scheduler lwp_get_scheduler(void){
    /*Returns the pointer to the current scheduler*/
    return rr;
}

void lwp_set_scheduler(scheduler sched){
    /*
    Causes the LWP package to use the given scheduler to choose the
    next process to run. Transfers all threads from the old scheduler
    to the new one in next() order. If scheduler is NULL the library
    should return to round-robin scheduling    
    */
    thread new_thread;
    int i;
    if (sched == NULL){
        rr = &rr_publish;
    }

    if (sched-> init != NULL){
        sched->init();
    }

    if (id_count == 1  && sched != NULL){
        rr = sched;
        return;
    }

    i = 1;
    thread temp = rr -> next();
    while(i < id_count) /*used for transferring threads from current scheduler to new one*/
    {
        new_thread = malloc(sizeof(context));
        new_thread-> stack = malloc(temp-> stacksize*sizeof(unsigned long));
        copy(temp, new_thread);
        rr -> remove(temp);
        sched -> admit(new_thread);
        temp =rr-> next();
        i++;
    }
    rr = sched; /*initializes running thread to new thread*/
}

int main(int argc, char *argv[]){
    return 0;
}