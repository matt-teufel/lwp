#include "lwp.h"
#include "rr_scheduler.h"
#include <stdlib.h>
#include <stdio.h>

thread head = NULL;
thread tail = NULL; /*used for tracking the end node*/
// struct scheduler rr_publish = {NULL, NULL, rr_admit, rr_remove, rr_next};

/*
sched_one: thread to be runned
sched_two: next thread to be runned

ex) sched_one <- thread <- sched_two

testing with main and thread structures

*/

void rr_admit(thread new);
void rr_remove(thread victim);
thread rr_next();
int count=0;
// int main()
// {
//     thread state = (thread) malloc(sizeof(context));
//     //thread state = NULL;
//     state -> tid = 5; 
//     state -> stack = (unsigned long*) malloc(5 * sizeof(unsigned long));
//     state -> stacksize = 5;
//     //printf("state tid value %i\n", state -> tid);

//     rr_admit(state);
//     printf("head tid value %i\n", head -> tid);
//     printf("head sched_one tid value %i\n", head -> sched_one -> tid);
//     //printf("head sched_two tid value %i\n", head ->sched_two  -> tid);

//     thread states = (thread) malloc(sizeof(context));
//     //thread state = NULL;
//     states -> tid = 1; 
//     states -> stack = (unsigned long*) malloc(5 * sizeof(unsigned long));
//     states -> stacksize = 7;

//     rr_admit(states);
//     printf("head tid value %i\n", head -> tid);
//     printf("head sched_one tid value %i\n", head -> sched_one -> tid);
//     printf("head sched_two tid value %i\n", head ->sched_two  -> tid);


//     return 0;
// }

void rr_admit(thread new){
    count++;
    /*admits a thread and adds to the tail of the head*/
    if (head == NULL){
        /*head has no threads*/
        head = new;
    }
    else{
        /*head has threads in list*/
        tail->sched_one = new;
    }
    new->sched_one = head;
    tail = new;
}


void rr_remove(thread victim){
    count--;
    thread temp;
    if (head == NULL){
        return;
    }
    /*first node removed*/
    if (victim->tid == head->tid){
        if(victim->sched_one != head){
            head = victim->sched_one;
            tail->sched_one = head;
        }else{
            head = NULL;
            tail = NULL;
            }
        return;
        }
    /*last node removed*/
    temp = head;
    while(temp->sched_one->tid != victim->tid){
        temp = temp->sched_one;
    }
    if(temp->sched_one->tid == tail->tid){
        tail = temp->sched_one->sched_one;
    }
    temp->sched_one = temp->sched_one->sched_one;
}



thread rr_next(){
    /*selects next thread to be scheduled
    may need to look into*/
    if(head){
        tail=head;
        head = head -> sched_one;
    }

    return head;
}

