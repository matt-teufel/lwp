#include "lwp.h"
#include <stdlib.h>
#include <stdio.h>

thread pool = NULL;

/*
sched_one: thread to be runned
sched_two: next thread to be runned

ex) sched_one <- thread <- sched_two

*/
void rr_admit(thread new){
    /*adds a thread to the pool
    look over to review*/
    if (pool){
        /*pool has threads already*/
        pool -> sched_two = new; /*stores new thread behind pool*/
        new -> sched_one = pool; /*new thread points to pool*/
    }
    else{
        pool = new;
        /*stores new as thread to be runned and next to be runned*/
        pool -> sched_one = new;
        pool -> sched_two = new;
    }
}

void rr_remove(thread victim){
    /*remove thread from pool*/
    victim -> sched_two -> sched_one = victim -> sched_one;
    victim -> sched_one -> sched_two = victim -> sched_two;

    if (victim -> tid == pool -> tid){
        if (victim  -> sched_one -> tid != victim -> tid){
            pool = victim -> sched_two;
            }
        else{
            pool = NULL;
        }
    }
}

thread rr_next(){
    /*selects thread to be scheduled*/
    if(pool){
        pool = pool -> sched_one;
    }
    return pool;
}

struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};
