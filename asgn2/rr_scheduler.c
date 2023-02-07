#include "lwp.h"
#include "rr_scheduler.h"
#include <stdlib.h>
#include <stdio.h>

thread pool = NULL;
thread ref = NULL; /*used for tracking the end node*/
struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};

/*
sched_one: thread to be runned
sched_two: next thread to be runned

ex) sched_one <- thread <- sched_two

testing with main and thread structures

*/

void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next();
// int main()
// {
//     thread state = (thread) malloc(sizeof(context));
//     //thread state = NULL;
//     state -> tid = 5; 
//     state -> stack = (unsigned long*) malloc(5 * sizeof(unsigned long));
//     state -> stacksize = 5;
//     //printf("state tid value %i\n", state -> tid);

//     rr_admit(state);
//     printf("pool tid value %i\n", pool -> tid);
//     printf("pool sched_one tid value %i\n", pool -> sched_one -> tid);
//     //printf("pool sched_two tid value %i\n", pool ->sched_two  -> tid);

//     thread states = (thread) malloc(sizeof(context));
//     //thread state = NULL;
//     states -> tid = 1; 
//     states -> stack = (unsigned long*) malloc(5 * sizeof(unsigned long));
//     states -> stacksize = 7;

//     rr_admit(states);
//     printf("pool tid value %i\n", pool -> tid);
//     printf("pool sched_one tid value %i\n", pool -> sched_one -> tid);
//     printf("pool sched_two tid value %i\n", pool ->sched_two  -> tid);


//     return 0;
// }

void rr_admit(thread new){
    /*admits a thread and adds to the ref of the pool*/
    if (pool == NULL){
        /*pool has no threads*/
        pool = new;
        new -> sched_two = NULL;
        new -> sched_one = pool;
    }
    else{
        /*pool has threads in list*/
        thread temp = pool;
        while(temp != ref){
            temp = temp -> sched_one;
        }
        temp -> sched_one = new;
        if(temp == pool){
            temp -> sched_two = new;
        }
        new -> sched_two = temp;
        new -> sched_one = pool;
    }
    ref = new;
}


void rr_remove(thread victim){

    if (!pool){
        thread temp = pool;
        thread temps = NULL;

        /*first node removed*/
        if (victim -> tid == pool -> tid){
            temp = pool;
            temps = NULL; /*holds thread after the removed one*/

            if(pool != pool -> sched_one){
                pool = pool -> sched_one;
            }
            else{
                pool = NULL;
            }

            temp -> sched_one = NULL;
            temp -> sched_two = NULL;
            if (pool != NULL){
                pool -> sched_two = ref;
                ref -> sched_one = pool;
            }
        }
    
        /*last node removed*/
        else if (victim -> tid == ref -> tid){
            temp = pool;
            temps = ref;
            while(temp != ref){
                temp = temp -> sched_one;
            }
            ref = temp;
            ref -> sched_one = pool;
            ref -> sched_two = NULL;
        }

        /*middle node removed*/
        else{
            temp = pool;
            temps = NULL; /*stores removed node*/
            while(temp != ref){
                temp = temp -> sched_one;
            }
            temps = temp -> sched_one;
            temp -> sched_one = temps -> sched_one; /*reconnects the pool*/
            temps -> sched_one = NULL;
            temps -> sched_two = NULL;
        }
    }
}

thread rr_next(){
    /*selects next thread to be scheduled
    may need to look into*/
    if(pool){
        pool = pool -> sched_one;
    }
    return pool;
}
