#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include "fp.h"
#include "waitq.h"
#include "exitq.h"
#include "allq.h"
#include "lwp.h"
#include "rr_scheduler.h"
#define EXIT_MASK 0xFF

struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};
scheduler rr = &rr_publish;
thread current_lwp = NULL; 
rfile *thread_context;
int id_count = 2;
int wait_count = 0;
int exit_count = 0;
int howbig = 8000000;

void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next();
void swap_rfiles(rfile *old, rfile *new);

thread tid2thread(tid_t tid){
    thread t = a_tid2thread(tid);
    if(t){
        return t;
    }
    return NO_THREAD;
}

void freet(thread t){
    if(t->stack){
        munmap(t->stack, t->stacksize);
    }
    munmap(t, sizeof(context));
}

tid_t lwp_wait(int *status){
    /*
    Waits for a thread to terminate, deallocates its resources, and reports its termination status if status is non-NULL.
    Returns the tid of the terminated thread or NO THREAD
    */
   // printf("start of wait\n");
    /* if not enough stuff has exited then we must 
    sleep and go on wait q */
    if(exit_count <= wait_count){
        wait_count++;
       // printf("wait -- exit count was lest than wait count ec: %i wc %i \n", exit_count, wait_count);
        rr->remove(current_lwp);
        // if(rr_qlen()==0){
        //     //prob free some stuff here


        //     printf("happy exit\n");
        //     exit(current_lwp->status);
        // }
        w_append(current_lwp);
        lwp_yield();
 //       printf("tid of sleepr: %i\n", (int)current_lwp->tid);
    }else if(wait_count){
       // printf("wait -- enough stuff has exitted for us but not our turn ec: %i wc %i \n", exit_count, wait_count);

        /* if enough stuff has exited but its not our turn
        we must wait, but we can stay on scheduler  */
        lwp_yield();
    }
    //reaping time
 //   printf("awake in wait\n");
 //   printf("tid of sleepr: %i\n", (int)current_lwp->tid);
    //printf("reap time  for %i\n",(int)current_lwp->tid);
    thread exited_head=e_pop();
    tid_t id = exited_head->tid;
    a_remove_thread(exited_head);
    if(exited_head->tid != 1){
        freet(exited_head); /* don't free main thread */
    }
    exit_count--;
  //  printf("end of wait\n");
    return id;
}

/* TODO 

use the termstat macro 

*/

void lwp_exit(int exitval){
    /*
    Terminates the current LWP and yields to whichever thread the
    scheduler chooses. lwp exit() does not return    
    */
 //   printf("lwp exit start\n");
    current_lwp->status |= (exitval & EXIT_MASK);
    rr->remove(current_lwp);
    e_append(current_lwp);
    exit_count++;
    if(w_first()){
        /* if someone is sleeping, wake them up */
   //     printf("waking someone up \n");
      //  printf("exit -- waking someone up: %i wc %i \n", exit_count, wait_count);
        rr->admit(w_pop());
        wait_count--;
    }
    lwp_yield();
}


tid_t lwp_create(lwpfun function,void * argument){
    /*Creates a new lightweight process which executes the given function
    with the given argument.
    lwp create() returns the (lightweight) thread id of the new thread
    or NO THREAD if the thread cannot be created. */

    thread new_thread;
    unsigned long *s;
    long page_size;
    //default stack size 8 MB
    page_size = getpagesize();
    struct rlimit limit_struct;
    getrlimit(RLIMIT_STACK, &limit_struct);
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
    if(s == MAP_FAILED || new_thread == MAP_FAILED){
        printf("unable to map region\n");
        return NO_THREAD;
    }
    new_thread->stacksize = howbig;
    // new_thread->stack=(s + (howbig/sizeof(unsigned long) - sizeof(unsigned long)));
    new_thread->stack = s;
  //  printf("tid: %i, stack p: %p, and its addr as long: %lu\n", (int)new_thread->tid,new_thread->stack,(unsigned long)(new_thread->stack));
    s=(new_thread->stack+ howbig/sizeof(unsigned long));
    if((unsigned long)s % 16 != 0){
        printf("stack is not alligned on 16 byte boundary\n");
    }
    new_thread->tid = id_count++;

    /*  
        initialize stack and registers as if 
        function called swap_rfiles() itself 
    */
    *(s-1) = (unsigned long)function; /* put function address on stack to mimic return address */
    *(s-2) = 0;          /* put rbp on stack */
    // s[-1] =(unsigned long)function;
    // s[-2] = (unsigned long)s;
    new_thread->state.fxsave=FPU_INIT; /*floating point state */
    new_thread->state.rbp = (unsigned long)(s - 2);     /* top of stack address */
    // new_thread->state.rsp = (unsigned long)(&s[-2]);     /* top of stack address */
    new_thread->state.rdi = (unsigned long)argument;/* argument */
    a_append(new_thread);
    rr -> admit(new_thread);
    return new_thread-> tid;
}


void lwp_start(void){
    /*
    Starts the threading system by converting the calling thread—the original system thread—into a
    LWP by allocating a context for it and admitting it to the scheduler, and yields control to whichever
    thread the scheduler indicates. It is not necessary to allocate a stack for this thread since it already
    has one
    */

    //thread_context = malloc(sizeof(rfile)); /*allocating context*/
    thread new_thread = mmap(NULL, sizeof(context),PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    new_thread->tid=1;
    new_thread->status = LWP_LIVE;
    //thread_context = mmap(NULL, sizeof(context),PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK,-1,0);
    //current_lwp = rr -> next(); /*creates a thread to be scheduled*/
  //  printf("lwp_start before swap files\n");
    swap_rfiles(&(new_thread->state), NULL); /*admit thread_context to scheduler, as thread to be scheduled*/
  //  printf("lwp start after swap files and before yield\n");
    rr->admit(new_thread);
    current_lwp = new_thread;
    lwp_yield();
  //  printf("lwp start after yield\n");
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
   //printf("start of yield\n");
   if (other_thread == NULL){
        swap_rfiles(NULL, &(current_lwp->state));
   }else if(current_lwp == NULL){
        printf("yield happy exit\n");
        exit(other_thread->status);   
    }else{
        swap_rfiles(&(other_thread -> state), &(current_lwp -> state));
   }
//   printf("end of yield\n");
}

tid_t lwp_gettid(void){
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

scheduler lwp_get_scheduler(void){
    /*Returns the pointer to the current scheduler*/
    return rr;
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
        temp = rr-> next();
        i++;
    }
    rr = sched; /*initializes running thread to new thread*/
}
